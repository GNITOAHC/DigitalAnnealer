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
void testSpin(int, Graph); // Cout index, graph and energy

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

Graph readInput () {
    double d;
    Graph graph;
    std::string line;
    std::vector<double> v;
    while (std::getline(std::cin, line)) {
        v.clear();
        std::stringstream ss(line);

        while (ss >> d)
            v.push_back(d);

        switch (v.size()) {
            case 1: graph.pushBack(v[0]); break;
            case 2: graph.pushBack(v[0], v[1]); break;
            case 3: graph.pushBack(v[0], v[1], v[2]); break;
            default: std::cerr << "Invalid input, size = " << v.size() << std::endl; exit(1);
        }
    }
    return graph;
}

void testSpin (int index, Graph graph) {
    graph.flipSpin(index);
    std::cout << "index " << index << " spined !! " << graph.getHamiltonianEnergy() << std::endl;
    return;
}
