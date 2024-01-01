#include <cmath>
#include <functional>
#include <random>
#include <tuple>

#include "Annealer.h"

// Randomly execute the given function with probability rand
bool Annealer::randomExec(const double rand, const std::function<void()> func) {
    // Random number generator
    std::mt19937 generator(std::random_device {}());
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    if (dis(generator) < rand) {
        func();
        return true;
    }

    // Fix the seed for testing
    // srand(0);
    // if ((double)std::rand() / ((double)RAND_MAX + 1) < rand) {
    //     func();
    //     return true;
    // }

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
        // graph.print();
    }
    return graph.getHamiltonianEnergy();
}
