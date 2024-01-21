#include "../graph/Graph.h"
#include "MpiAnnealer.h"

#include <cmath>
#include <functional>
#include <mpi.h>
#include <random>
#include <tuple>

typedef std::function<double(double&, double&, double&, double&)> deltaSGenFunc;
bool swap(const int, double, double, std::vector<Spin>&, deltaSGenFunc); // myrank, compare_src1, compare_src2, spin config

bool configCompare (double& src1, double& src2, double& target1, double& target2, deltaSGenFunc deltaS_func) {
    const double deltaS = deltaS_func(src1, src2, target1, target2);

    std::mt19937 generator(std::random_device {}());
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    const double prob = exp(deltaS);
    const double rand_num = (double)dis(generator);

    if (prob > rand_num) return true;
    return false;
}

bool swap (const int myrank, double cmp_src1, double cmp_src2, std::vector<Spin>& config, deltaSGenFunc deltaS_func) {
    MPI_Request requests = MPI_REQUEST_NULL;
    MPI_Status status;

    int src_a = myrank % 2 == 0 ? myrank : myrank - 1;
    int src_b = myrank % 2 == 0 ? myrank + 1 : myrank;

    int tag_a = src_a;
    int tag_b = src_b;

    double cmp_src1_other, cmp_src2_other;

    // Check if src_a and src_b is swapping
    bool is_swap = false;
    int config_size = config.size();
    // Create buffer for getting config if swapping src_a and src_b
    std::vector<Spin> buffer(config_size);

    if (myrank == src_a) {
        // MPI sending gamma & energy
        MPI_Send(&cmp_src1, 1, MPI_DOUBLE, src_b, tag_a, MPI_COMM_WORLD);
        MPI_Wait(&requests, &status);

        MPI_Send(&cmp_src2, 1, MPI_DOUBLE, src_b, tag_a, MPI_COMM_WORLD);
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
        MPI_Recv(&cmp_src1_other, 1, MPI_DOUBLE, src_a, tag_a, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Wait(&requests, MPI_STATUS_IGNORE);

        MPI_Recv(&cmp_src2_other, 1, MPI_DOUBLE, src_a, tag_a, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Wait(&requests, MPI_STATUS_IGNORE);

        // Compare config check if swapping config is needed
        is_swap = configCompare(cmp_src1, cmp_src2, cmp_src1_other, cmp_src2_other, deltaS_func);

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
