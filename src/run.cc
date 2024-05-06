#include "./args/Args.h"
#include "./graph/tri/tri.h"
#include "run.h"

#include <cstring>
#include <iomanip>
#include <ios>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdarg.h>
#include <vector>

#include "./algo/sa/sa.h"
#include "./algo/sqa/sqa.h"
#include "./include/AnnealFunc.h"

#define debug(n) std::cerr << n << std::endl;

// Make graph with length, height and gamma
Graph makeGraph(const int&, const int&, const double& gamma);
// String format
std::string format (const std::string fmt_str, ...) {
    int final_n, n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
    std::unique_ptr<char[]> formatted;
    va_list ap;
    while (1) {
        formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
        strcpy(&formatted[0], fmt_str.c_str());
        va_start(ap, fmt_str);
        final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
        va_end(ap);
        if (final_n < 0 || final_n >= n) n += abs(final_n - n + 1);
        else break;
    }
    return std::string(formatted.get());
}

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

    // Check function to run & set tau
    const std::string func = args.hasArg("--func") ? std::get<std::string>(args.getArg("--func")) : "sa";
    const int tau = args.hasArg("--tau") ? std::get<int>(args.getArg("--tau")) : 100000;

    std::fstream file;
    Graph graph;

    /*
     * Debug section start
     */
    file.open(std::get<std::string>(args.getArg("--file")), std::ios::in);
    graph = readInput(file);
    if (file.is_open()) file.close();
    graph.lockLength();
    struct Params_SA params = {
        .rank = myrank,
        .init_t = temp,
        .final_t = final_temp,
        .tau = tau,
    };
    Anlr_SA sqa(graph, params);
    std::ofstream outfile;

    std::cout << "hamiltonian_energy: " << sqa.getHamiltonianEnergy() << std::endl;

    outfile.open("tmp.txt", std::ios::out);
    sqa.printConfig(outfile);
    outfile.close();

    sqa.anneal();

    outfile.open("tmp1.txt", std::ios::out);
    sqa.printConfig(outfile);
    outfile.close();

    std::cout << "hamiltonian_energy: " << sqa.getHamiltonianEnergy() << std::endl;

    return 0;
    /*
     * Debug section end
     */

    /*
     * Grow layer count for simulated quantum annealing
     */
    int grow_layer = 7; // Grow 7 layers i.e. 8 layers in total

    if (args.hasArg("--h-tri")) {
        /*
         * Build graph for triangular lattice
         */
        const int tri_width = std::get<int>(args.getArg("--h-tri"));
        // graph = makeGraph(v[0], 1, gamma); // makeGraph(length, height, gamma)
        graph = tri::makeGraph(tri_width); // makeGraph(length)
        graph.lockLength(tri_width * tri_width);
        if (args.hasArg("--height")) grow_layer = std::get<int>(args.getArg("--height")) - 1;
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
        if (args.hasArg("--height")) grow_layer = std::get<int>(args.getArg("--height")) - 1;
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
    // const std::vector<double> squared_ops = tri::getSquaredOP(graph);
    // std::cout << "Squared order parameter debug section START:\n";
    // for (int i = 0; i < squared_ops.size(); ++i) {
    //     printf("layer %d: %f\n", i, squared_ops[i]);
    // }
    // std::cout << std::endl;

    // Create an Annealer
    Annealer annealer(myrank);
    if (func == "sa") { // Simulated annealing
        std::cout << "Simulated annealing" << std::endl;
        const double hamiltonian_energy = annealer.annealTemp(std::make_tuple(temp, tau, final_temp), graph);
        std::cout << "Hamiltonian energy: " << hamiltonian_energy << std::endl;
    } else if (func == "sqa") { // Simulated quantum annealing
        std::cout << "Simulated quantum annealing" << std::endl;
        graph.growLayer(grow_layer, gamma);
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

    /*
     * --print-conf
     *  Print to L<len>_H<height>_Ti<init-t>_Tf<final-t>_tau<tau>.tsv <- Simulated Annealing
     *  Print to L<len>_H<height>_Gi<init-g>_Gf<final-g>_tau<tau>.tsv <- Simulated Quantum Annealing
     *  Print to tri_<len>_<height>_Ti<init-t>_Tf<final-t>_tau<tau>.tsv <- Triangular lattice for Simulated Annealing
     *  Print to tri_<len>_<height>_Gi<init-g>_Gf<final-g>_tau<tau>.tsv <- Triangular lattice for Simulated Quantum Annealing
     */
    std::string filenames[4] = { "L%d_H%d_Ti%f_Tf%f_tau%d.tsv", "L%d_H%d_Gi%f_Gf%f_tau%d.tsv", "tri_%d_%d_Ti%f_Tf%f_tau%d.tsv",
                                 "tri_%d_%d_Gi%f_Gf%f_tau%d.tsv" };
    const int length = graph.getLength(), height = graph.getHeight();
    if (args.hasArg("--print-conf")) {
        std::string filename;
        std::ofstream outfile;
        /*
         * Print the config to file for general purpose.
         */
        if (func == "sa") {
            filename = format(filenames[0], length, height, temp, final_temp, tau);
        } else if (func == "sqa") {
            filename = format(filenames[1], length, height, gamma, final_gamma, tau);
        }
        outfile.open(filename, std::ios::out);
        graph.printHLayer(outfile);
        outfile.close();
        /*
         * Print the config to file.
         */
        if (func == "sa") {
            filename = format("cfg_" + filenames[0], length, height, temp, final_temp, tau);
        } else if (func == "sqa") {
            filename = format("cfg_" + filenames[1], length, height, gamma, final_gamma, tau);
        }
        outfile.open(filename, std::ios::out);
        graph.printConfig(outfile);
        outfile.close();
        /*
         * Print the config to file for triangular lattice.
         */
        if (args.hasArg("--h-tri")) {
            if (func == "sa") {
                filename = format(filenames[2], length, height, temp, final_temp, tau);
            } else if (func == "sqa") {
                filename = format(filenames[3], length, height, gamma, final_gamma, tau);
            }
            outfile.open(filename, std::ios::out);
            tri::printTriConf(graph, outfile);
            outfile.close();
        }
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
