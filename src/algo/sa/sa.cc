#include "./sa.h"
#include <cmath>

#ifdef USE_MPI
#include "../../annealer/MpiAnnealer.h"
#endif

// Grph_SA Constructor
Anlr_SA::Grph_SA::Grph_SA () : Graph() {
    return;
}
Anlr_SA::Grph_SA::Grph_SA (const Graph& g) : Graph(g) {
    return;
}

// Anlr_SA getParams
Params_SA Anlr_SA::getParams () const {
    return this->params;
}

// Anlr_SA Constructor
Anlr_SA::Anlr_SA () : Annealer(0), graph() {
    return;
}
Anlr_SA::Anlr_SA (const Graph& g, const Params_SA& p) : Annealer(p.rank), graph(g), params(p) {
    return;
}

// Anlr_SA Reexported functions from Graph
int Anlr_SA::getLength () const {
    return this->graph.getLength();
}
int Anlr_SA::getHeight () const {
    return this->graph.getHeight();
}
std::vector<Spin> Anlr_SA::getSpins () const {
    return this->graph.getSpins();
}

// Anlr_SQA Printer
void Anlr_SA::printConfig (std::ofstream& out) const {
    return this->graph.printConfig(out);
}
void Anlr_SA::printHLayer (std::ofstream& out) const {
    return this->graph.printHLayer(out);
}

// Anlr_SA anneal
double Anlr_SA::anneal () {
    const double temp0 = this->params.init_t, final_temp = this->params.final_t;
    const int tau = this->params.tau;
    for (int i = 0; i <= tau; ++i) {
        const double T   = temp0 * (1 - ((double)i / tau)) + final_temp * ((double)i / tau);
        const int length = graph.spins.size();
        for (int j = 0; j < length; ++j) {
            // Calculate the PI_accept
            const double delta_E   = graph.getHamiltonianDifference(j);
            const double PI_accept = std::min(1.0, std::exp(-delta_E / T));

            // Flip the spin with probability PI_accept
            this->randomExec(PI_accept, [&] () { graph.flipSpin(j); });

            if (print_progress) std::cout << T << " " << graph.getHamiltonianEnergy() << std::endl;
        }

#ifdef USE_MPI
        deltaSGenFunc deltaS = [] (double& src_temp, double& src_energy, double& target_temp,
                                   double& target_energy) {
            return ((1 / target_temp) - (1 / src_temp)) * (target_energy - src_energy);
        };
        if (i % 8 == 0) {
            std::vector<Spin> config = this->graph.getSpins();
            swap(this->myrank, T, graph.getHamiltonianEnergy(), config, deltaS);
        }
#endif
    }
    return this->graph.getHamiltonianEnergy();
}

// Anlr_SA getHamiltonianEnergy
double Anlr_SA::getHamiltonianEnergy () const {
    return this->graph.getHamiltonianEnergy();
}

// Anlr_SA deltaS
double Anlr_SA::deltaS (double& src_temp, double& src_energy, double& target_temp,
                        double& target_energy) {
    return ((1 / target_temp) - (1 / src_temp)) * (target_energy - src_energy);
}
