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
    run(argc, argv, 0);
    return 0;
}
