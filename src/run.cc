#include "./args/Args.h"
#include "./graph/tri/tri.h"
#include "run.h"

#include <iomanip>
#include <ios>
#include <iostream>
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
    CustomArgs args(argc, argv);

    // Get the initial configuration values
    double gamma = args.hasArg("--ini-g") ? std::get<double>(args.getArg("--ini-g")) : 2.0;
    double final_gamma = args.hasArg("--final-g") ? std::get<double>(args.getArg("--final-g")) : 0.0;
    double temp = args.hasArg("--ini-t") ? std::get<double>(args.getArg("--ini-t")) : 2.0;
    double final_temp = args.hasArg("--final-t") ? std::get<double>(args.getArg("--final-t")) : 0.0;

    std::fstream file;
    Graph graph;

    if (args.hasArg("--h-tri")) {
        /*
         * Build graph for triangular lattice
         */
        std::vector<int> v = std::get<std::vector<int> >(args.getArg("--h-tri"));
        // graph = makeGraph(v[0], 1, gamma); // makeGraph(length, height, gamma)
        graph = tri::makeGraph(v[0]); // makeGraph(length)
        graph.lockLength(v[0] * v[0]);
        graph.growLayer(v[1] - 1, gamma);
    } else {
        /*
         * Build graph for general purpose
         */
        const bool is_qubo = args.hasArg("--qubo"); // Convert to ising if it's QUBO

        file.open(std::get<std::string>(args.getArg("--file")), std::ios::in);
        graph = is_qubo ? readInputFromQubo(file) : readInput(file);
        if (file.is_open()) file.close();

        // Lock the length of the graph after reading the input
        graph.lockLength();
        const int height = args.hasArg("--height") ? std::get<int>(args.getArg("--height")) : 4;
        graph.growLayer(height - 1, gamma);
    }

    /*
     * Debug section
     */
    // std::ofstream outfile;
    // std::string filename = "output" + std::to_string(myrank) + ".txt";
    // outfile.open(filename, std::ios::out);
    // graph.print(outfile);
    // outfile.close();

    std::cout << std::setprecision(10); // Set precision to 10 digits
    std::cout << "Hamiltonian energy: " << graph.getHamiltonianEnergy() << std::endl;

    /*
     * Debug section
     */
    // return 0;

    // Check function to run & set tau
    const std::string func = args.hasArg("--func") ? std::get<std::string>(args.getArg("--func")) : "sa";
    const int tau = args.hasArg("--tau") ? std::get<int>(args.getArg("--tau")) : 100000;

    /*
     * Debug section
     */
    // const std::vector<double> squared_ops = tri::getSquaredOP(graph);
    // std::cout << "Squared order parameter debug section START:\n";
    // for (int i = 0; i < squared_ops.size(); ++i) {
    //     printf("layer %d: %f\n", i, squared_ops[i]);
    // }
    // std::cout << std::endl;

    // Create an Annealer
    Annealer annealer(myrank);
    if (func == "sa") { // Simulated annealing
        const double hamiltonian_energy = annealer.annealTemp(std::make_tuple(temp, tau, final_temp), graph);
        std::cout << "Hamiltonian energy: " << hamiltonian_energy << std::endl;
    } else if (func == "sqa") { // Simulated quantum annealing
        std::cout << "Simulated quantum annealing" << std::endl;
        const double hamiltonian_energy = annealer.annealGamma(std::make_tuple(gamma, tau, final_gamma), graph);
        std::cout << "Hamiltonian energy: " << hamiltonian_energy << std::endl;
    }

    /*
     * Debug section ( Hamiltonian energy of each layer )
     */
    // std::vector<double> list_of_energy = graph.getLayerHamiltonianEnergy();
    // std::cout << "Layer Hamiltonian energy:\n";
    // for (int i = 0; i < list_of_energy.size(); ++i)
    //     printf("layer %d: %f\n", i, list_of_energy[i]);
    // std::cout << std::endl;

    // Can only be used when the graph is a triangular graph (--h-tri)
    if (args.hasArg("--h-tri")) {
        /*
         * Debug section ( Squared order parameters of each layer )
         */
        const std::vector<double> squared_ops = tri::getSquaredOP(graph);
        std::cout << "Squared order parameter debug section END:\n";
        for (int i = 0; i < squared_ops.size(); ++i) {
            printf("layer %d: %f\n", i, squared_ops[i]);
        }
        std::cout << std::endl;
    }

    if (args.hasArg("--print-conf")) {
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
