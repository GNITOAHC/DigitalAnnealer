#include <iomanip>
#include <iostream>
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

Graph readInput () {
    double co;
    int po1, po2;
    Graph graph;
    while (std::cin >> po1 >> po2 >> co) {
        graph.pushBack(po1, po2, co);
    }
    return graph;
}

void testSpin (int index, Graph graph) {
    graph.flipSpin(index);
    std::cout << "index " << index << " spined !! " << graph.getHamiltonianEnergy() << std::endl;
    return;
}

int main (int argc, char **argv) {
    Args args(argc, argv);

    // Get the triangular length and height if specified
    const bool is_tri = std::get<0>(args.getTri());
    int triangular_length = 0, triangular_height = 0;
    if (is_tri) { std::tie(triangular_length, triangular_height) = std::get<1>(args.getTri()); }
    // Get whether or not the graph is a QUBO ( convert to ising if it's QUBO )
    const bool is_qubo = args.isQubo();

    // Debug session
    if (is_tri) {
        debug(triangular_height);
        debug(triangular_length);
    }
    if (is_qubo) { debug("QUBO"); }

    Graph graph = readInput();

    std::cout << std::setprecision(10); // Set precision to 10 digits

    std::cout << "Hamiltonian energy: " << graph.getHamiltonianEnergy() << std::endl;
    // graph.print();
    Annealer annealer;
    std::cout << "Hamiltonian energy: " << annealer.anneal(std::make_tuple(10, 10000), graph) << std::endl;
    // testSpin(4, graph);
    // graph.print();
    // std::cout << graph.getHamiltonianEnergy() << std::endl;
    return 0;
}
