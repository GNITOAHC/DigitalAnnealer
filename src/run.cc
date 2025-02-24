#include "./args/Args.h"
#include "./graph/tri/tri.h"
#include "./runhelper.h"
#include "run.h"

#include <cfloat>
#include <iomanip>
#include <ios>
#include <iostream>
#include <sstream>
#include <stdarg.h>
#include <variant>
#include <vector>

#include "./algo/sa/sa.h"
#include "./algo/sqa/sqa.h"
#include "graph/Graph.h"

#define debug(n) std::cerr << n << std::endl;

// Make graph with length, height and gamma
Graph makeGraph(const int&, const int&, const double& gamma);

void readSpins (const std::string& filename, Anlr_SA& graph) {
    std::vector<Spin> spins;
    std::fstream file;
    file.open(filename, std::ios::in);
    std::string line;
    std::vector<int> v;
    int i;

    while (std::getline(file, line)) {
        if (line[0] == '#') continue; // Skip comment line
        v.clear();
        std::stringstream ss(line);

        while (ss >> i)
            v.push_back(i);

        switch (v.size()) {
            case 2: graph.setSpins(v[0], v[1]); break;
            default: break;
        }
    }
}

int run (int argc, char **argv, const int myrank) {
#ifdef USE_MPI
    printf("rank = %d\n", myrank);
#endif
    CustomArgs args(argc, argv);
    ANNEAL_FUNC strategy = args.getStrategy();

    if (strategy == NIL) strategy = SA; // Default strategy is SA

    std::fstream file;
    Graph graph;

    if (args.hasArg("--h-tri")) {
        /*
         * Build graph for triangular lattice
         */
        const int tri_width = std::get<int>(args.getArg("--h-tri"));
        // graph = makeGraph(v[0], 1, gamma); // makeGraph(length, height, gamma)
        graph               = tri::makeGraph(tri_width); // makeGraph(length)
        graph.lockLength(tri_width * tri_width);
    } else {
        /*
         * Build graph for general purpose
         */
        file.open(std::get<std::string>(args.getArg("--file")), std::ios::in);
        graph = args.hasArg("--qubo") ? readInputFromQubo(file)
                                      : readInput(file); // Convert to Ising if it's QUBO
        if (file.is_open()) file.close();

        // Lock the length of the graph after reading the input
        graph.lockLength();
    }

    std::cout << std::setprecision(10); // Set precision to 10 digits
    std::cout << "Hamiltonian energy: " << graph.getHamiltonianEnergy() << std::endl;
    switch (strategy) {
        case SA: std::cout << "Simulated Annealing" << std::endl; break;
        case SQA: std::cout << "Simulated Quantum Annealing" << std::endl; break;
        default: break;
    }

    std::variant<Params_SA, Params_SQA> prms;
    std::variant<Anlr_SA, Anlr_SQA> anlr;
    double hamiltonian_energy = DBL_MAX;

    int rank_count = 1;
    if (args.hasArg("--ans-count")) rank_count = std::get<int>(args.getArg("--ans-count"));

    for (int rank = 0; rank < rank_count; ++rank) {
        switch (strategy) {
            case SA:
                {
                    struct Params_SA params = { .rank = rank };
                    if (args.hasArg("--ini-t"))
                        params.init_t = std::get<double>(args.getArg("--ini-t"));
                    if (args.hasArg("--final-t"))
                        params.final_t = std::get<double>(args.getArg("--final-t"));
                    if (args.hasArg("--tau")) params.tau = std::get<int>(args.getArg("--tau"));
                    Anlr_SA sa(graph, params);

                    if (args.hasArg("--print-progress")) sa.print_progress = true;

                    if (args.hasArg("--spin-conf")) {
                        std::string filename = std::get<std::string>(args.getArg("--spin-conf"));
                        readSpins(filename, sa);
                    }

                    hamiltonian_energy = sa.anneal();

                    anlr = sa;
                    prms = params;

                    break;
                }
            case SQA:
                {
                    struct Params_SQA params = { .rank = rank };
                    if (args.hasArg("--ini-g"))
                        params.init_g = std::get<double>(args.getArg("--ini-g"));
                    if (args.hasArg("--final-g"))
                        params.final_g = std::get<double>(args.getArg("--final-g"));
                    if (args.hasArg("--tau")) params.tau = std::get<int>(args.getArg("--tau"));
                    if (args.hasArg("--height"))
                        params.layer_count = std::get<int>(args.getArg("--height"));
                    if (args.hasArg("--gamma"))
                        params.gamma = std::get<double>(args.getArg("--gamma"));
                    Anlr_SQA sqa(graph, params);
                    hamiltonian_energy = sqa.anneal();

                    anlr = sqa;
                    prms = params;

                    break;
                }
            default: break;
        }

        std::cout << hamiltonian_energy << std::endl;

        if (!args.hasArg("--print-conf")) continue; // Program end if --print-conf is not set

        switch (strategy) {
            case SA: printSAV2(std::get<Anlr_SA>(anlr), std::get<Params_SA>(prms)); break;
            case SQA: printSQA(std::get<Anlr_SQA>(anlr), std::get<Params_SQA>(prms)); break;
            default: break;
        }

        // Print config to file for triangular lattice
        if (args.hasArg("--h-tri")) {
            std::string filename;
            switch (strategy) {
                case SA: printTriSA(std::get<Anlr_SA>(anlr), std::get<Params_SA>(prms)); break;
                case SQA: printTriSQA(std::get<Anlr_SQA>(anlr), std::get<Params_SQA>(prms)); break;
                default: break;
            }
        }
    }

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
        if (line[0] == '#') continue; // Skip comment line
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
