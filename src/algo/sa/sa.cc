#include "./sa.h"

// Grph_SA Constructor
Anlr_SA::Grph_SA::Grph_SA(const Graph& g) : Graph(g) {
    std::cout << "Grph_SQA Constructor" << std::endl;
    return;
}

// Anlr_SA Constructor
Anlr_SA::Anlr_SA(const Graph& g, const Params_SA& p) : Annealer(p.rank), graph(g), params(p) {
    std::cout << "Anlr_SA Constructor" << std::endl;
    return;
}

// Anlr_SQA printConfig
void Anlr_SA::printConfig(std::ofstream& out) { return this->graph.printConfig(out); }

// Anlr_SA anneal
double Anlr_SA::anneal() {
    const double temp0 = this->params.init_t, final_temp = this->params.final_t;
    const int tau = this->params.tau;
    for (int i = 0; i <= tau; ++i) {
        const double T = temp0 * (1 - ((double)i / tau)) + final_temp * ((double)i / tau);
        const int length = graph.spins.size();
        for (int j = 0; j < length; ++j) {
            // Calculate the PI_accept
            const double delta_E = graph.getHamiltonianDifference(j);
            const double PI_accept = std::min(1.0, std::exp(-delta_E / T));

            // Flip the spin with probability PI_accept
            randomExec(PI_accept, [&] () { graph.flipSpin(j); });
        }
    }
    return this->graph.getHamiltonianEnergy();
}

// Anlr_SA getHamiltonianEnergy
double Anlr_SA::getHamiltonianEnergy() const { return this->graph.getHamiltonianEnergy(); }
