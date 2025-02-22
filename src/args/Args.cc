#include "./Args.h"

#include <iostream>
#include <vector>

using namespace argparse;

CustomArgs::CustomArgs (const int argc, char **argv) : Args(argc, argv, this->argsConstruct()) {
    if (this->hasArg("--help")) this->outputHelp();
    this->addConstraint(this->argsConstraint());
    this->customConstraintsCheck();
    return;
}
CustomArgs::CustomArgs (const int argc, char **argv, const std::vector<ArgFormat>& af)
    : Args(argc, argv, af) {}

std::vector<ArgFormat> CustomArgs::argsConstruct () const {
    return std::vector<struct ArgFormat>({
        // key, type, arg_count, required
        { "--qubo", ARG_BOOL, 0 }, // Transform the input to QUBO
        { "--file", ARG_STRING, 1 }, // The source file path of the input
        { "--h-tri", ARG_INT, 1 }, // Specify a triangular lattice ( Hamiltonian )
        { "--ini-g", ARG_DOUBLE, 1 }, // Specify a initial gamma value for triangular lattice
        { "--final-g", ARG_DOUBLE, 1 }, // Specify a final gamma value
        { "--ini-t", ARG_DOUBLE, 1 }, // Specify a initial temperature value for triangular lattice
        { "--final-t", ARG_DOUBLE, 1 }, // Specify a final temperature value
        { "--tau", ARG_INT, 1 }, // Specify a tau for annealer
        { "--func", ARG_STRING,
         1 }, // Either "sa" for simulated annealing or "sqa" for simulated quantum annealing
        { "--height", ARG_INT,
         1 }, // Specify a height for triangular lattice ( When annealing with func sqa ) default 4
        { "--ans-count", ARG_INT, 1 }, // Specify a number of answers to be returned
        { "--print-conf", ARG_BOOL, 0 }, // Print the configuration
        { "--print-progress", ARG_BOOL, 0 }, // Print the configuration
        { "--help", ARG_BOOL, 0, false }, // Display help
    });
};

std::vector<struct argparse::ArgConstraint> CustomArgs::argsConstraint () const {
    return std::vector<struct ArgConstraint>({
        // key, constraint, co_exist
        // { "--qubo", "--file", COEXIST },
        { "--h-tri", "--file", MUTEX },
        { "--h-tri", "--qubo", MUTEX },
        // { "--h-tri", "--ini-g", REQUIRE },
    });
}

void CustomArgs::customConstraintsCheck () const {
    if (!this->hasArg("--h-tri") && !this->hasArg("--file")) {
        throw std::invalid_argument("Either --h-tri or --file must be specified");
    }
    if (this->hasArg("--func")) {
        const std::string func = std::get<std::string>(this->getArg("--func"));
        if (func != "sa" && func != "sqa") {
            std::cout << func << "is not a valid function option" << std::endl;
            throw std::invalid_argument("Invalid function specified");
        }
    }
    // if (this->hasArg("--func") && std::get<std::string>(this->getArg("--func")) == "sqa") {
    //     if (this->hasArg("--h-tri") && std::get<std::vector<int> >(this->getArg("--h-tri"))[1] <=
    //     1) {
    //         throw std::invalid_argument("Invalid operation, height could not be less than 2");
    //     }
    // }
    return;
}

ANNEAL_FUNC CustomArgs::getStrategy () const {
    if (!this->hasArg("--func")) return ANNEAL_FUNC::NIL;
    if (std::get<std::string>(this->getArg("--func")) == "sa") return ANNEAL_FUNC::SA;
    if (std::get<std::string>(this->getArg("--func")) == "sqa") return ANNEAL_FUNC::SQA;
    return NIL;
}

void CustomArgs::outputHelp () const {
    // std::cout << "Usage: " << this->argv[0] << " [options]" << std::endl;
    // clang-format off
    std::cout << "Options:" << std::endl;
    std::cout << "  --qubo                     Specify that the graph is a QUBO" << std::endl;
    std::cout << "  --file <source>            The source file path of the input hamiltonian" << std::endl;
    std::cout << "  --h-tri <length>           Use built-in tool to create triangular lattice" << std::endl;
    std::cout << "  --ini-g <gamma>            Specify an initial gamma value for triangular lattice" << std::endl;
    std::cout << "  --final-g <gamma>          Specify an final gamma value for triangular lattice" << std::endl;
    std::cout << "  --ini-t <temp>             Specify an initial temperature value for triangular lattice" << std::endl;
    std::cout << "  --final-t <temp>           Specify an final temperature value for triangular lattice" << std::endl;
    std::cout << "  --tau <tau>                Specify a tau for annealer" << std::endl;
    std::cout << "  --func <func_string>       Specify a function for annealer, either \"sa\" or \"sqa\" " << std::endl;
    std::cout << "  --height <height>          Specify a height for triangular lattice ( When annealing with func sqa ) default 8" << std::endl;
    std::cout << "  --print-progress           Print the annealing progress" << std::endl;
    std::cout << "  --print-conf               Output the configuration" << std::endl;
    std::cout << "  --help                     Display this information" << std::endl;
    // clang-format on
    exit(0);
}
