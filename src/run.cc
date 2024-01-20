#include "Helper.h"
#include "run.h"

#include <iomanip>
#include <ios>
#include <iostream>
#include <math.h>
#include <sstream>
#include <vector>

#define debug(n) std::cerr << n << std::endl;

const double E = std::exp(1.0);

inline double loge (double x) { return std::log(x) / std::log(E); }

// Make graph with length, height and gamma
Graph makeGraph(const int&, const int&, const double& gamma);

int run (int argc, char **argv, const int myrank) {
#ifdef USE_MPI
    printf("rank = %d\n", myrank);
#endif
    Args args(argc, argv);

    int triangular_length = 0, triangular_height = 0;
    double gamma = 0;

    // Get the triangular length and height if specified
    const bool is_tri = std::get<0>(args.getTri());
    if (is_tri) { std::tie(triangular_length, triangular_height) = std::get<1>(args.getTri()); }

    // Get whether or not the graph is a QUBO ( convert to ising if it's QUBO )
    const bool is_qubo = args.isQubo();

    // Get length, height and gamma if specified
    const bool is_builtin_tri = std::get<0>(args.useDefault());
    std::fstream file;
    Graph graph;
    // Detect if use file input or default triangular input
    if (is_builtin_tri) {
        std::tie(triangular_length, triangular_height) = std::get<1>(args.useDefault());
        gamma = std::get<2>(args.useDefault());
        graph = makeGraph(triangular_length, triangular_height, gamma);
    } else {
        file.open(args.getSourceFile(), std::ios::in);
        graph = is_qubo ? readInputFromQubo(file) : readInput(file);
    }

    if (file.is_open()) file.close();

    // graph.print();
    // exit(0);

    std::cout << std::setprecision(10); // Set precision to 10 digits

    std::cout << "Hamiltonian energy: " << graph.getHamiltonianEnergy() << std::endl;

    // {
    //     Annealer annealer;
    //     const int temperature_tau = args.getTemperatureTau() != 0 ? args.getTemperatureTau() : 100000;
    //     const double hamiltonian_energy = annealer.annealTemp(std::make_tuple(10, temperature_tau), graph);
    //     std::cout << "Hamiltonian energy: " << hamiltonian_energy << std::endl;
    // }

    {
        Annealer annealer(myrank);
        annealer.myrank = myrank;
        const int gamma_tau = 100;
        const double hamiltonian_energy =
            annealer.annealGamma(std::make_tuple(gamma, gamma_tau), graph, std::make_tuple(triangular_length, triangular_height));
        std::cout << "Hamiltonian energy: " << hamiltonian_energy << std::endl;
    }

    // Can only be used when the graph is a triangular graph
    if (is_tri) {
        // const double op_length_square = graph.getOrderParameterLengthSquared(triangular_length, triangular_height);
        // std::cout << "Order parameter length squared: " << op_length_square << std::endl;
        const std::vector<double> list_of_op_length_square = graph.getOrderParameterLengthSquared(triangular_length, triangular_height);
        std::cout << "Layer order parameter length squared:\n";
        for (int i = 0; i < list_of_op_length_square.size(); ++i) {
            printf("layer %d: %f\n", i, list_of_op_length_square[i]);
        }
        std::cout << std::endl;

        // std::vector<double> list_of_energy = graph.getLayerHamiltonianEnergy(triangular_height);
        // std::cout << "Layer Hamiltonian energy:\n";
        // for (int i = 0; i < list_of_energy.size(); ++i)
        //     printf("layer %d: %f\n", i, list_of_energy[i]);
        // std::cout << std::endl;

        // graph.print();
    }

    // testSpin(4, graph);
    // graph.print();
    // std::cout << graph.getHamiltonianEnergy() << std::endl;
    return 0;
}

Graph makeGraph (const int& length, const int& height, const double& gamma) {
    Graph graph;
    const int length2 = length * length;
    const double strength_between_layer = gamma == 0.0 ? 0.0 : (-0.5) * loge(tanh(gamma));
    int (*macro_array[])(const int&, const int&, const int&, const int&) = { GETRIGHT, GETBOTTOM, GETBOTTOMRIGHT };
    for (int h = 0; h < height; ++h) {
        for (int i = 0; i < length; ++i) {
            for (int j = 0; j < length; ++j) {
                const int index = h * length2 + i * length + j;
                for (int c = 0; c < 3; ++c)
                    graph.pushBack(index, (*macro_array[c])(h, i, j, length), 1.0);
                if (height > 1) graph.pushBack(index, GETLAYERUP(h, i, j, length, height), strength_between_layer);
            }
        }
    }

    return graph;
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