#include <mpi.h>

#include "run.h"

int main (int argc, char **argv) {
    int myrank, nprocs;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    // printf("Hello from processor %d of %d\n", myrank, nprocs);

    run(argc, argv);

    MPI_Finalize();
    return 0;
}
