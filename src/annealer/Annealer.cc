#include <cmath>
#include <functional>
#include <random>
#include <tuple>

#include "Annealer.h"

#ifdef USE_MPI
#include "MpiAnnealer.h"
#endif

Annealer::Annealer(const int r) : myrank(r) {}

double tempDeltaS (double& src_temp, double& src_energy, double& target_temp, double& target_energy) {
    return ((1 / target_temp) - (1 / src_temp)) * (target_energy - src_energy);
}
double gammaDeltaS (double& src_gamma, double& src_energy, double& target_gamma, double& target_energy) {
    return (target_gamma - src_gamma) * (target_energy - src_energy);
}

// Randomly execute the given function with probability rand
bool Annealer::randomExec(const double rand, const std::function<void()> func) {
    // Random number generator
    std::mt19937 generator(std::random_device {}());
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    if (dis(generator) < rand) {
        func();
        return true;
    }

    return false;
}

double Annealer::annealTemp(std::tuple<double, double> temperature, Graph& graph) {
    const auto [T0, tau] = temperature;
    for (int i = 0; i < tau; ++i) {
        const double T = T0 * (1 - ((double)i / tau));
        const int length = graph.spins.size();
        for (int j = 0; j < length; ++j) {
            // Calculate the PI_accept
            const double delta_E = graph.getHamiltonianDifference(j);
            const double PI_accept = std::min(1.0, std::exp(-delta_E / T));

            // Flip the spin with probability PI_accept
            randomExec(PI_accept, [&] () { graph.flipSpin(j); });
        }
#ifdef USE_MPI
        if (i % 8 == 0) {
            // MPI swap spins by temperature and hamiltonian energy
            std::vector<Spin> config = graph.getSpins();

            // MPI communication
            swap(this->myrank, T, graph.getHamiltonianEnergy(), config, tempDeltaS);
        }
#endif
    }
    return graph.getHamiltonianEnergy();
}

double Annealer::annealGamma(const std::tuple<double, double>& gamma, Graph& graph, const std::tuple<int, int>& graph_size) {
    const auto [gamma0, tau] = gamma;
    for (int i = 0; i < tau; ++i) {
        const double gamma = gamma0 * (1 - ((double)i / tau));
        const int length = graph.spins.size();
        for (int j = 0; j < length; ++j) {
            // Calculate the PI_accept
            const double delta_E = graph.getHamiltonianDifference(j);
            const double PI_accept = std::min(1.0, std::exp(-delta_E));

            // Flip the spin with probability PI_accept
            randomExec(PI_accept, [&] () { graph.flipSpin(j); });
        }
        // Update the gamma: gamma, length, height
        graph.updateGamma(gamma, std::get<0>(graph_size), std::get<1>(graph_size));
#ifdef USE_MPI
        if (i % 8 == 0) {
            // MPI swap spins by temperature and hamiltonian energy
            std::vector<Spin> config = graph.getSpins();
            // printf("Config before swap: ");
            // for (int i = 0; i < config.size(); ++i)
            //     printf("%d ", config[i]);
            std::vector<double> vertical_energy_list = graph.getVerticalEnergyProduct(std::get<1>(graph_size));
            double vertical_energy_sum = 0.0;
            for (const auto& energy : vertical_energy_list) {
                vertical_energy_sum += energy;
            }

            // MPI communication
            swap(this->myrank, gamma, vertical_energy_sum, config, gammaDeltaS);
            // printf("Config: ");
            // for (int i = 0; i < config.size(); ++i)
            //     printf("%d ", config[i]);
            // printf("\n");
        }
#endif
    }
    return graph.getHamiltonianEnergy();
}
