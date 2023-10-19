#include "Args.h"
#include <iostream>
#include <string>

/* Private helper */
bool Args::isNumber(const std::string& s) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it))
        ++it;
    return !s.empty() && it == s.end();
}

/* Private parser */
void Args::parseArgs() {
    for (int i = 1; i < this->argc; ++i) {
        // Parse triangular lattice arguments
        if (this->argv[i] == std::string("--tri")) {
            if (i + 2 >= this->argc) {
                std::cerr << "Error: --tri requires two arguments" << std::endl;
                exit(1);
            }
            if (!isNumber(this->argv[i + 1]) || !isNumber(this->argv[i + 2])) {
                std::cerr << "Error: --tri requires two integers" << std::endl;
                exit(1);
            }
            this->tri_length = atoi(this->argv[i + 1]);
            this->tri_height = atoi(this->argv[i + 2]);
        }
        // Parse QUBO argument
        if (this->argv[i] == std::string("--qubo")) { this->is_qubo = true; }
        // Parse help argument
        if (this->argv[i] == std::string("--help")) {
            std::cout << "Usage: " << this->argv[0] << " [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --tri <length> <height>  Specify a triangular lattice" << std::endl;
            std::cout << "  --qubo                   Specify that the graph is a QUBO" << std::endl;
            std::cout << "  --help                   Display this information" << std::endl;
            exit(0);
        }
    }
    return;
}

/* Constructor */
Args::Args(const int argc, char **argv) {
    this->argc = argc;
    this->argv = argv;
    this->tri_length = 0;
    this->tri_height = 0;
    this->is_qubo = false;
    this->parseArgs();
    return;
}

/* Getters */
std::tuple<bool, std::pair<int, int> > Args::getTri() const {
    return std::make_tuple(this->tri_length != 0 && this->tri_height != 0, std::make_pair(this->tri_length, this->tri_height));
}

bool Args::isQubo() const { return this->is_qubo; }
