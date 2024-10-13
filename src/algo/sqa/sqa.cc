#include "sqa.h"
#include <cmath>
#include <numeric>

#ifdef USE_MPI
#include "../../annealer/MpiAnnealer.h"
#endif

// Grph_SQA Constructor
Anlr_SQA::Grph_SQA::Grph_SQA () : Graph() {
    return;
}
Anlr_SQA::Grph_SQA::Grph_SQA (const Graph& g) : Graph(g) {
    return;
}

// Grph_SQA getParams
Params_SQA Anlr_SQA::getParams () const {
    return this->params;
}

// Grph_SQA updateGamma
void Anlr_SQA::Grph_SQA::updateGamma (const double& gamma) {
    char gamma_update_flag = 0X00; // check if gamma is updated for both up and down
    const int length       = this->getLength();
    const int height       = this->spins.size() / length;
    for (int i = 0; i < adj_list.size(); ++i) {
        AdjNode *tmp             = adj_list[i];
        const int next_layer_idx = (i + length) % (length * height);
        const int prev_layer_idx = (i - length) >= 0 ? i - length : i % length;
        while (tmp != nullptr) {
            if (!(gamma_update_flag ^ 0X03)) { // if gamma_update_flag == 0X03
                break;
            } else if (tmp->val == next_layer_idx) {
                tmp->weight = gamma;
                gamma_update_flag |= 1;
            } else if (tmp->val == prev_layer_idx) {
                tmp->weight = gamma;
                gamma_update_flag |= 2;
            }
            tmp = tmp->next;
        }
        gamma_update_flag = 0X00; // reset gamma_update_flag
    }
    return;
}

// Reexported functions from Graph
int Anlr_SQA::getLength () const {
    return this->graph.getLength();
}
int Anlr_SQA::getHeight () const {
    return this->graph.getHeight();
}
std::vector<Spin> Anlr_SQA::getSpins () const {
    return this->graph.getSpins();
}

// Grph_SQA growLayer
const double E = std::exp(1.0);
inline double loge (double x) {
    return std::log(x) / std::log(E);
}
void Anlr_SQA::Grph_SQA::growLayer (const int& grow_count, const double& gamma) {
    const int length = this->getLength();
    const int height = this->spins.size() / length, origin_constant = this->constant / height;
    const double g = (-0.5) * loge(tanh(gamma));
    for (int i = 0; i < grow_count; ++i) {
        // Duplicate the current layer to the new layer
        for (int j = 0; j < length; ++j) {
            const int index = (i + 1) * length + j; // New layer's index
            // Add edge between the new layer
            AdjNode *tmp    = adj_list[j];
            while (tmp != nullptr) {
                const int corr_node = tmp->val + (length * (i + 1));
                if (tmp->val == j + length) { // Prevent from adding edge to the next layer
                    tmp = tmp->next;
                    continue;
                }
                // Prevent from adding duplicate edge
                if (index < corr_node) this->pushBack(index, corr_node, tmp->weight);
                tmp = tmp->next;
            }
            // Add constant map of the new layer
            if (constant_map.find(j) != constant_map.end()) {
                this->pushBack(index, constant_map[j]);
            }
        }
        this->constant += origin_constant; // Add constant of the new layer
        // Add edge between the new layer & the previous layer
        for (int j = 0; j < length; ++j) {
            const int index          = i * length + j; // Previous layer's index
            const int next_layer_idx = (index + length) % (length * (grow_count + 1));
            // std::cout << "index = " << index << " next_layer_idx = " << next_layer_idx <<
            // std::endl;
            this->pushBack(index, next_layer_idx, g);
        }
    }
    // Link back to the first layer
    for (int j = 0; j < length; ++j) {
        const int index          = grow_count * length + j;
        const int next_layer_idx = j;
        // std::cout << "index = " << index << " next_layer_idx = " << next_layer_idx << std::endl;
        this->pushBack(index, next_layer_idx, g);
    }
    return;
}

// Anlr_SQA Constructor
Anlr_SQA::Anlr_SQA () : Annealer(0), graph() {}
Anlr_SQA::Anlr_SQA (const Graph& g, const int& rank) : Annealer(rank), graph(g) {}
Anlr_SQA::Anlr_SQA (const Graph& g, const Params_SQA& params)
    : Annealer(params.rank), graph(g), params(params) {}

// Anlr_SQA growLayer
void Anlr_SQA::growLayer (const int& grow_count, const double& gamma) {
    return this->graph.growLayer(grow_count, gamma);
}

// Anlr_SQA Printer
void Anlr_SQA::printConfig (std::ofstream& out) const {
    return this->graph.printConfig(out);
}
void Anlr_SQA::printHLayer (std::ofstream& out) const {
    return this->graph.printHLayer(out);
}

// Anlr_SQA anneal
double Anlr_SQA::anneal () {
    this->graph.growLayer(this->params.layer_count - 1, this->params.gamma);
    const double gamma0 = this->params.init_g, final_gamma = this->params.final_g;
    const int tau = this->params.tau;

    for (int i = 0; i <= tau; ++i) {
        const double gamma = gamma0 * (1 - ((double)i / tau)) + final_gamma * ((double)i / tau);
        // const int length = this->graph.getSpinSize();
        const int length   = graph.spins.size();
        for (int j = 0; j < length; ++j) {
            // Calculate the PI_accept
            const double delta_E   = graph.getHamiltonianDifference(j);
            const double PI_accept = std::min(1.0, std::exp(-delta_E));

            // Flip the spin with probability PI_accept
            this->randomExec(PI_accept, [&] () { graph.flipSpin(j); });
        }
        // Update the gamma: gamma, length, height
        graph.updateGamma(gamma);

#ifdef USE_MPI
        deltaSGenFunc deltaS = [] (double& src_gamma, double& src_energy, double& target_gamma,
                                   double& target_energy) -> double {
            return (target_gamma - src_gamma) * (target_energy - src_energy);
        };
        if (i % 8 == 0) {
            std::vector<Spin> config   = graph.getSpins();
            double vertical_energy_sum = this->getVerticalEnergySum();
            swap(this->params.rank, gamma, vertical_energy_sum, config, deltaS);
        }
#endif
    }

    return this->graph.getHamiltonianEnergy();
}

// Anlr_SQA getHamiltonianEnergy
double Anlr_SQA::getHamiltonianEnergy () const {
    return this->graph.getHamiltonianEnergy();
}

// Anlr_SQA getVerticalEnergySum
double Anlr_SQA::getVerticalEnergySum () const {
    const int length = this->graph.getLength();
    std::vector<double> list_of_energy(length, 0.0);

    for (int i = 0; i < length; ++i) {
        const int self_idx = this->graph.adj_list[i]->val;
        AdjNode *tmp       = this->graph.adj_list[i];
        // int current_layer = 0;

        // \sum_{i=1}^L { \sum_{l=1}^{L_tau} { s_i^l * s_i^{l+1} } }
        while (tmp->val != self_idx) {
            const int layer_up_idx = (tmp->val + length) % (length * this->params.layer_count);
            list_of_energy[i] +=
                (double)this->graph.spins[tmp->val] * (double)this->graph.spins[layer_up_idx];
            // ++current_layer;
        }
    }

    return std::accumulate(list_of_energy.begin(), list_of_energy.end(), 0.0);
}
