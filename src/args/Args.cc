#include "./Args.h"

#include <iostream>
#include <vector>

using namespace argparse;

CustomArgs::CustomArgs(const int argc, char **argv) : Args(argc, argv, this->argsConstruct()) {
    if (this->hasArg("--help")) this->outputHelp();
    this->addConstraint(this->argsConstraint());
    this->customConstraintsCheck();
    return;
}
CustomArgs::CustomArgs(const int argc, char **argv, const std::vector<ArgFormat>& af) : Args(argc, argv, af) {}

std::vector<ArgFormat> CustomArgs::argsConstruct() const {
    return std::vector<struct ArgFormat>({
        // key, type, arg_count, required
        { "--tri", ARG_VI, 2 },
        { "--qubo", ARG_BOOL, 0 },
        { "--file", ARG_STRING, 1 },
        { "--default-tri", ARG_VI, 2 },
        { "--gamma", ARG_DOUBLE, 1 },
        { "--final-gamma", ARG_DOUBLE, 1 },
        { "--tau", ARG_INT, 1 },
        { "--func", ARG_STRING, 1 }, // Either "sa" or "sqa"
        { "--print", ARG_BOOL, 0 },
        { "--help", ARG_BOOL, 0, false },
    });
};

std::vector<struct argparse::ArgConstraint> CustomArgs::argsConstraint() const {
    return std::vector<struct ArgConstraint>({
        // key, constraint, co_exist
        { "--default-tri", "--file", MUTEX },
        { "--default-tri", "--gamma", COEXIST },
        { "--default-tri", "--qubo", MUTEX },
    });
}

void CustomArgs::customConstraintsCheck() const {
    if (!this->hasArg("--default-tri") && !this->hasArg("--file")) {
        throw std::invalid_argument("Either --default-tri or --file must be specified");
    }
    if (this->hasArg("--func")) {
        const std::string func = std::get<std::string>(this->getArg("--func"));
        if (func != "temp" && func != "gamma") {
            std::cout << func << "is not a valid function option" << std::endl;
            throw std::invalid_argument("Invalid function specified");
        }
    }
    return;
}

void CustomArgs::outputHelp() const {
    // std::cout << "Usage: " << this->argv[0] << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --tri <length> <height>          Specify a triangular lattice" << std::endl;
    std::cout << "  --qubo                           Specify that the graph is a QUBO" << std::endl;
    std::cout << "  --file <source>                  The source file path of the input" << std::endl;
    std::cout << "  --default-tri <length> <height>  Use built-in tool to create triangular lattice" << std::endl;
    std::cout << "  --gamma <gamma>                  Specify a gamma value for triangular lattice" << std::endl;
    std::cout << "  --final-gamma <gamma>            Specify a final gamma value" << std::endl;
    std::cout << "  --tau <tau>                      Specify a tau for annealer" << std::endl;
    std::cout << "  --func <func_string>             Specify a function for annealer, either \"sa\" or \"sqa\" " << std::endl;
    std::cout << "  --print                          Output the configuration" << std::endl;
    std::cout << "  --help                           Display this information" << std::endl;
    exit(0);
}
