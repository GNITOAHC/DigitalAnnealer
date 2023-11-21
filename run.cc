#include <iomanip>
#include <ios>
#include <iostream>
#include <sstream>
#include <vector>

#include "run.h"

int run (int argc, char **argv) {
    Args args(argc, argv);

    // Get the triangular length and height if specified
    const bool is_tri = std::get<0>(args.getTri());
    int triangular_length = 0, triangular_height = 0;
    if (is_tri) { std::tie(triangular_length, triangular_height) = std::get<1>(args.getTri()); }
    // Get whether or not the graph is a QUBO ( convert to ising if it's QUBO )
    const bool is_qubo = args.isQubo();

    // Debug session
    if (is_tri) { debug("TRI"); }
    if (is_qubo) { debug("QUBO"); }

    std::fstream file;
    file.open(args.getSourceFile(), std::ios::in);

    Graph graph = is_qubo ? readInputFromQubo(file) : readInput(file);

    if (file.is_open()) file.close();

    std::cout << std::setprecision(10); // Set precision to 10 digits

    std::cout << "Hamiltonian energy: " << graph.getHamiltonianEnergy() << std::endl;

    Annealer annealer;
    std::cout << "Hamiltonian energy: " << annealer.anneal(std::make_tuple(10, 10000), graph) << std::endl;

    // Can only be used when the graph is a triangular graph
    if (is_tri) {
        const double op_length_square = graph.getOrderParameterLengthSquared(triangular_length, triangular_height);
        std::cout << "Order parameter length squared: " << op_length_square << std::endl;
    }

    // testSpin(4, graph);
    // graph.print();
    // std::cout << graph.getHamiltonianEnergy() << std::endl;
    return 0;
}

Graph readInputFromQubo (std::fstream& source) {
    double d;
    Graph graph;
    std::string line;
    std::vector<double> v;
    while (std::getline(source, line)) {
        v.clear();
        std::stringstream ss(line);

        while (ss >> d)
            v.push_back(d);

        switch (v.size()) {
            case 1: graph.pushBack(v[1]); break;
            case 2:
                graph.pushBack(v[0], v[1] / 2); // pushBack(k/2, index)
                graph.pushBack(v[1] / 2);
                break;
            case 3: // v[0]: po1, v[1]: po2, v[2]: co
                graph.pushBack(v[0], v[1], v[2] / 4);
                graph.pushBack(v[0], v[2] / 4);
                graph.pushBack(v[1], v[2] / 4);
                graph.pushBack(v[2] / 4);
                break;
            default: std::cerr << "Invalid input, size = " << v.size() << std::endl; exit(1);
        }
    }
    return graph;
}

Graph readInput (std::fstream& source) {
    double d;
    Graph graph;
    std::string line;
    std::vector<double> v;
    while (std::getline(source, line)) {
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
