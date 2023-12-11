#include "Args.h"
#include <iostream>

/* Private helper */
bool Args::isNumber(const std::string& s) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it))
        ++it;
    return !s.empty() && it == s.end();
}

void Args::outputHelp() const {
    std::cout << "Usage: " << this->argv[0] << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --tri <length> <height>          Specify a triangular lattice" << std::endl;
    std::cout << "  --qubo                           Specify that the graph is a QUBO" << std::endl;
    std::cout << "  --file <source>                  The source file path of the input" << std::endl;
    std::cout << "  --default-tri <length> <height>  Use built-in tool to create triangular lattice" << std::endl;
    std::cout << "  --gamma <gamma>                  Specify a gamma value for triangular lattice" << std::endl;
    std::cout << "  --temperature-tau <tau>          Specify a temperature tau" << std::endl;
    std::cout << "  --help                           Display this information" << std::endl;
    exit(0);
}

void Args::validateInput(std::set<std::string>& argument_set) const {
    std::set<std::string>::iterator tri_it, qubo_it, file_it, default_tri_it, gamma_it;

    const bool is_tri = argument_set.find("--tri") == argument_set.end() ? false : true;
    const bool is_qubo = argument_set.find("--qubo") == argument_set.end() ? false : true;
    const bool have_file = argument_set.find("--file") == argument_set.end() ? false : true;
    const bool use_default = argument_set.find("--default-tri") == argument_set.end() ? false : true;
    const bool have_gamma = argument_set.find("--gamma") == argument_set.end() ? false : true;

    // --default-tri can't use with --tri, --qubo or --file
    if ((is_tri && use_default) || (is_qubo && use_default) || (have_file && use_default)) {
        std::cerr << "Error: --tri argument or --file can't be used together with --default-tri" << std::endl;
        this->outputHelp();
    }

    // --qubo and --tri can't appear together
    if (is_tri && is_qubo) {
        std::cerr << "Error: --tri and --qubo can't appear together" << std::endl;
        this->outputHelp();
    }

    // --gamma is required when --use-default and height larger than 1
    if (!have_gamma && use_default && this->tri_height > 1) {
        std::cerr << "Error: Specify a gamma value with default triangular lattice" << std::endl;
        this->outputHelp();
    }

    // At least specify one kind of input with --default-tri or --file
    if (!use_default && !have_file) {
        std::cerr << "Error: Use --defult-tri or --file to specify an input" << std::endl;
        this->outputHelp();
    }
}

/* Private parser */
void Args::parseArgs() {
    std::set<std::string> argument_set;

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
            argument_set.insert("--tri");
        }

        // Parse QUBO argument
        if (this->argv[i] == std::string("--qubo")) {
            this->is_qubo = true;
            argument_set.insert("--qubo");
        }

        // Parse source file arguments
        if (this->argv[i] == std::string("--file")) {
            if (i + 1 >= this->argc) {
                std::cerr << "Error: --file requires one source file path" << std::endl;
                exit(0);
            }
            this->source_file = std::string(this->argv[i + 1]);
            argument_set.insert("--file");
        }

        // Default triangular lattice
        if (this->argv[i] == std::string("--default-tri")) {
            if (i + 2 >= this->argc) {
                std::cerr << "Error: --default-tri requires two arguments" << std::endl;
                exit(0);
            }
            if (!isNumber(this->argv[i + 1]) || !isNumber(this->argv[i + 2])) {
                std::cerr << "Error: --default-tri requires two integers" << std::endl;
                exit(0);
            }
            this->tri_length = atoi(this->argv[i + 1]);
            this->tri_height = atoi(this->argv[i + 2]);
            this->use_builtin = true;
            argument_set.insert("--default-tri");
        }

        // Parse gamma argument
        if (this->argv[i] == std::string("--gamma")) {
            if (i + 1 >= this->argc) {
                std::cerr << "Error: --gamma requires one value" << std::endl;
                exit(0);
            }
            this->tri_gamma = atof(this->argv[i + 1]);
            argument_set.insert("--gamma");
        }

        // Parse temperature tau argument
        if (this->argv[i] == std::string("--temperature-tau")) {
            if (i + 1 >= this->argc) {
                std::cerr << "Error: --temperature-tau requires one value" << std::endl;
                exit(0);
            }
            if (!isNumber(this->argv[i + 1])) {
                std::cerr << "Error: --temperature-tau requires one integer" << std::endl;
                exit(0);
            }
            this->temperature_tau = atof(this->argv[i + 1]);
            argument_set.insert("--temperature-tau");
        }

        // Parse help argument
        if (this->argv[i] == std::string("--help")) { this->outputHelp(); }
    }

    // Check argument conflicts
    this->validateInput(argument_set);

    return;
}

/* Constructor */
Args::Args(const int argc, char **argv) : argc(argc), argv(argv) {
    this->tri_length = 0;
    this->tri_height = 0;
    this->tri_gamma = 0.0;
    this->temperature_tau = 0;
    this->use_builtin = false;
    this->is_qubo = false;
    this->source_file = "";
    this->parseArgs();
    return;
}

/* Getters */
std::tuple<bool, std::pair<int, int> > Args::getTri() const {
    return std::make_tuple(this->tri_length != 0 && this->tri_height != 0, std::make_pair(this->tri_length, this->tri_height));
}

std::tuple<bool, std::pair<int, int>, double> Args::useDefault() const {
    return std::make_tuple(this->use_builtin, std::make_pair(this->tri_length, this->tri_height), this->tri_gamma);
}

bool Args::isQubo() const { return this->is_qubo; }

int Args::getTemperatureTau() const { return this->temperature_tau; }

std::string Args::getSourceFile() const { return this->source_file; }
