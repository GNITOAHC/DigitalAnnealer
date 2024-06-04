#ifdef USE_MPI
#include <mpi.h>
#endif

#include "run.h"
#define debug(n) std::cerr << n << std::endl;

/*
 * function -> camalCase
 * variable -> snake_case
 * class -> PascalCase
 * struct -> PascalCase
 * constant -> ALL_CAPS
 * global variable -> ALL_CAPS
 */

int main (int argc, char **argv) {
    int myrank = 0;

#ifdef USE_MPI
    int nprocs = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
#endif

    run(argc, argv, myrank);

#ifdef USE_MPI
    MPI_Finalize();
#endif

    return 0;
}
