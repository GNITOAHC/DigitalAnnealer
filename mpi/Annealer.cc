#include "Annealer.h"

#include <cmath>
#include <functional>
#include <mpi.h>
#include <random>
#include <tuple>

static const std::mt19937 generator(std::random_device {}());
static std::uniform_real_distribution<double> dis(0.0, 1.0);

// Randomly execute the given function with probability rand
bool Annealer::randomExec(const double rand, const std::function<void()> func) {
    // Random number generator
    // std::mt19937 generator(std::random_device {}());
    // std::uniform_real_distribution<double> dis(0.0, 1.0);

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

bool tempConfigCompare (double& src_temp, double& src_energy, double& target_temp, double& target_energy) {
    // "[formula]" beta = inverse temperature: Beta = 1 / T
    const double deltaS = ((1 / target_temp) - (1 / src_temp)) * (target_energy - src_energy);

    const double prob = exp(deltaS);
    const double rand_num = (double)dis(generator);

    if (prob > rand_num) return true;
    return false;
}

bool gammaConfigCompare (double& src_gamma, double& src_energy, double& target_gamma, double& target_energy) {
    const double deltaS = (target_gamma - src_gamma) * (target_energy - src_energy);

    const double prob = exp(deltaS);
    const double rand_num = (double)dis(generator);

    if (prob > rand_num) return true;
    return false;
}

bool Annealer::tempSwap(int myrank, double temp_rank, double hami_rank, std::vector<Spin>& config) {
    MPI_Request requests = MPI_REQUEST_NULL;
    MPI_Status status;

    int src_a, src_b;

    if (myrank % 2 == 0) {
        src_a = myrank;
        src_b = myrank + 1;
    } else {
        src_a = myrank - 1;
        src_b = myrank;
    }

    int tag_a = src_a;
    int tag_b = src_b;

    double temp_other;
    double hami_other;

    // Check if src_a and src_b is swapping
    bool is_swap = false;
    int config_size = config.size();
    // Create buffer for getting config if swapping src_a and src_b
    std::vector<Spin> buffer(config_size);

    if (myrank == src_a) {
        // MPI sending temperature & hamiltonian
        MPI_Send(&temp_rank, 1, MPI_DOUBLE, src_b, tag_a, MPI_COMM_WORLD);
        MPI_Wait(&requests, &status);

        MPI_Send(&hami_rank, 1, MPI_DOUBLE, src_b, tag_a, MPI_COMM_WORLD);
        MPI_Wait(&requests, &status);

        // Receive to know if two config is swapped
        MPI_Recv(&is_swap, 1, MPI_CXX_BOOL, src_b, tag_b, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if (is_swap) {
            MPI_Send(&config[0], config_size, MPI_INT, src_b, tag_a, MPI_COMM_WORLD);
            MPI_Wait(&requests, &status);

            MPI_Recv(&buffer[0], config_size, MPI_INT, src_b, tag_b, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else {
            // no swap
        }
    } else if (myrank == src_b) {
        // MPI receiving temperature & hamiltonian
        MPI_Recv(&temp_other, 1, MPI_DOUBLE, src_a, tag_a, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Wait(&requests, MPI_STATUS_IGNORE);

        MPI_Recv(&hami_other, 1, MPI_DOUBLE, src_a, tag_a, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Wait(&requests, MPI_STATUS_IGNORE);

        // Compare config check if swapping config is needed
        is_swap = tempConfigCompare(temp_rank, hami_rank, temp_other, hami_other);

        MPI_Send(&is_swap, 1, MPI_CXX_BOOL, src_a, tag_b, MPI_COMM_WORLD);
        MPI_Wait(&requests, &status);

        // If the config need to be swapped, send the config to src_a
        if (is_swap) {
            MPI_Send(&config[0], config_size, MPI_INT, src_a, tag_b, MPI_COMM_WORLD);
            MPI_Wait(&requests, &status);

            MPI_Recv(&buffer[0], config_size, MPI_INT, src_a, tag_a, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else {
            // no swap
        }
    }

    if (is_swap) { config = buffer; }

    return is_swap;
}

bool Annealer::gammaSwap(int myrank, double gamma_rank, double energy_rank, std::vector<Spin>& config) {
    MPI_Request requests = MPI_REQUEST_NULL;
    MPI_Status status;

    int src_a, src_b;

    if (myrank % 2 == 0) {
        src_a = myrank;
        src_b = myrank + 1;
    } else {
        src_a = myrank - 1;
        src_b = myrank;
    }

    int tag_a = src_a;
    int tag_b = src_b;

    double gamma_other;
    double hami_other;

    // Check if src_a and src_b is swapping
    bool is_swap = false;
    int config_size = config.size();
    // Create buffer for getting config if swapping src_a and src_b
    std::vector<Spin> buffer(config_size);

    if (myrank == src_a) {
        // MPI sending gamma & energy
        MPI_Send(&gamma_rank, 1, MPI_DOUBLE, src_b, tag_a, MPI_COMM_WORLD);
        MPI_Wait(&requests, &status);

        MPI_Send(&energy_rank, 1, MPI_DOUBLE, src_b, tag_a, MPI_COMM_WORLD);
        MPI_Wait(&requests, &status);

        // Receive to know if two config is swapped
        MPI_Recv(&is_swap, 1, MPI_CXX_BOOL, src_b, tag_b, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if (is_swap) {
            MPI_Send(&config[0], config_size, MPI_INT, src_b, tag_a, MPI_COMM_WORLD);
            MPI_Wait(&requests, &status);

            MPI_Recv(&buffer[0], config_size, MPI_INT, src_b, tag_b, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else {
            // no swap
        }
    } else if (myrank == src_b) {
        // MPI receiving gamma & energy
        MPI_Recv(&gamma_other, 1, MPI_DOUBLE, src_a, tag_a, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Wait(&requests, MPI_STATUS_IGNORE);

        MPI_Recv(&hami_other, 1, MPI_DOUBLE, src_a, tag_a, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Wait(&requests, MPI_STATUS_IGNORE);

        // Compare config check if swapping config is needed
        is_swap = gammaConfigCompare(gamma_rank, energy_rank, gamma_other, hami_other);

        MPI_Send(&is_swap, 1, MPI_CXX_BOOL, src_a, tag_b, MPI_COMM_WORLD);
        MPI_Wait(&requests, &status);

        // If the config need to be swapped, send the config to src_a
        if (is_swap) {
            MPI_Send(&config[0], config_size, MPI_INT, src_a, tag_b, MPI_COMM_WORLD);
            MPI_Wait(&requests, &status);

            MPI_Recv(&buffer[0], config_size, MPI_INT, src_a, tag_a, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else {
            // no swap
        }
    }

    if (is_swap) { config = buffer; }

    return is_swap;
}

double Annealer::annealTemp(std::tuple<double, double> temperature, Graph& graph) {
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    // srand(0);
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

        if (i % 8 == 0) {
            // MPI swap spins by temperature and hamiltonian energy
            std::vector<Spin> config = graph.getSpins();

            // MPI communication
            tempSwap(myrank, T, graph.getHamiltonianEnergy(), config);
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

        if (i % 8 == 0) {
            // MPI swap spins by temperature and hamiltonian energy
            std::vector<Spin> config = graph.getSpins();
            std::vector<double> vertical_energy_list = graph.getVerticalEnergyProduct(std::get<1>(graph_size));
            double vertical_energy_sum = 0.0;
            for (const auto& energy : vertical_energy_list) {
                vertical_energy_sum += energy;
            }

            // MPI communication
            gammaSwap(myrank, gamma, vertical_energy_sum, config);
        }
    }
    return graph.getHamiltonianEnergy();
}
