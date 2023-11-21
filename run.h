#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include "Annealer.h"
#include "Args.h"
#include "Graph.h"

#define debug(n) std::cerr << n << std::endl;

/*
 * function -> camalCase
 * variable -> snake_case
 * class -> PascalCase
 * struct -> PascalCase
 * constant -> ALL_CAPS
 * global variable -> ALL_CAPS
 */

/* Helper functions */

Graph readInput();
Graph readInputFromQubo();
void testSpin(int, Graph); // Cout index, graph and energy

int run(int, char**);
