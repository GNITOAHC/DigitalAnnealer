#include "./args/Args.h"
#include "./include/Helper.h"
#include "run.h"

#include <iomanip>
#include <ios>
#include <iostream>
#include <math.h>
#include <sstream>
#include <vector>

#define debug(n) std::cerr << n << std::endl;

const double E = std::exp(1.0);
const int T0 = 2;

inline double loge (double x) { return std::log(x) / std::log(E); }

// Make graph with length, height and gamma
Graph makeGraph(const int&, const int&, const double& gamma);

int run (int argc, char **argv, const int myrank) {
#ifdef USE_MPI
    printf("rank = %d\n", myrank);
#endif
    CustomArgs args(argc, argv);

    int triangular_length = 0, triangular_height = 0;
    double gamma = 0;

    // Get whether or not the graph is a QUBO ( convert to ising if it's QUBO )
    const bool is_qubo = args.hasArg("--qubo");
    // Get whether or not the graph is a triangular graph
    const bool is_tri = args.hasArg("--tri") || args.hasArg("--default-tri");

    std::fstream file;
    Graph graph;

    // Set the graph to triangular graph if the option is specified
    if (is_tri) {
        std::vector<int> v =
            args.hasArg("--tri") ? std::get<std::vector<int> >(args.getArg("--tri")) : std::get<std::vector<int> >(args.getArg("--default-tri"));
        triangular_length = v[0], triangular_height = v[1];
    }

    // Read input from file or use default
    if (args.hasArg("--file")) {
        file.open(std::get<std::string>(args.getArg("--file")), std::ios::in);
        graph = is_qubo ? readInputFromQubo(file) : readInput(file);
        if (file.is_open()) file.close();
    } else if (args.hasArg("--default-tri")) {
        gamma = std::get<double>(args.getArg("--gamma"));
        graph = makeGraph(triangular_length, triangular_height, gamma);
    }

    std::cout << std::setprecision(10); // Set precision to 10 digits

    std::cout << "Hamiltonian energy: " << graph.getHamiltonianEnergy() << std::endl;

    const std::string func = args.hasArg("--func") ? std::get<std::string>(args.getArg("--func")) : "sa";
    const int tau = args.hasArg("--tau") ? std::get<int>(args.getArg("--tau")) : 100000;

    if (func == "sa") { // Simulated annealing
        Annealer annealer(myrank);
        const double hamiltonian_energy = annealer.annealTemp(std::make_tuple(T0, tau), graph);
        std::cout << "Hamiltonian energy: " << hamiltonian_energy << std::endl;
    } else if (func == "sqa") { // Simulated quantum annealing
        Annealer annealer(myrank);
        annealer.myrank = myrank;
        const double final_gamma = args.hasArg("--final-gamma") ? std::get<double>(args.getArg("--final-gamma")) : 0.0;
        const double hamiltonian_energy =
            annealer.annealGamma(std::make_tuple(gamma, tau, final_gamma), graph, std::make_tuple(triangular_length, triangular_height));
        std::cout << "Hamiltonian energy: " << hamiltonian_energy << std::endl;
    }

    // Can only be used when the graph is a triangular graph
    if (is_tri) {
        // const double op_length_square = graph.getOrderParameterLengthSquared(triangular_length, triangular_height);
        // std::cout << "Order parameter length squared: " << op_length_square << std::endl;
        const std::vector<double> list_of_op_length_square = graph.getOrderParameterLengthSquared(triangular_length, triangular_height);
        std::cout << "Squared order parameter:\n";
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

    if (args.hasArg("--print")) {
        std::ofstream outfile;
        std::string filename = "output" + std::to_string(myrank) + ".txt";
        outfile.open(filename, std::ios::out);
        graph.print(outfile);
        outfile.close();
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
