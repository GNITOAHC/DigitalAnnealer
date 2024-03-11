#include "Args.h"
#include <iostream>
#include <sstream>

// void Args::outputHelp() const {
//     std::cout << "Usage: " << this->argv[0] << " [options]" << std::endl;
//     std::cout << "Options:" << std::endl;
//     std::cout << "  --tri <length> <height>          Specify a triangular lattice" << std::endl;
//     std::cout << "  --qubo                           Specify that the graph is a QUBO" << std::endl;
//     std::cout << "  --file <source>                  The source file path of the input" << std::endl;
//     std::cout << "  --default-tri <length> <height>  Use built-in tool to create triangular lattice" << std::endl;
//     std::cout << "  --gamma <gamma>                  Specify a gamma value for triangular lattice" << std::endl;
//     std::cout << "  --temperature-tau <tau>          Specify a temperature tau" << std::endl;
//     std::cout << "  --gamma-tau <tau>                Specify a gamma tau" << std::endl;
//     std::cout << "  --final-gamma <gamma>            Specify a final gamma value" << std::endl;
//     std::cout << "  --help                           Display this information" << std::endl;
//     exit(0);
// }

namespace ArgConstructor {
std::vector<ArgFormat> argsConstruct () {
    return std::vector<ArgFormat>({ // key, type, arg_count, required
                                    { "--tri", ARG_VI, 2 },
                                    { "--qubo", ARG_BOOL, 0 },
                                    { "--file", ARG_STRING, 1 },
                                    { "--default-tri", ARG_VI, 2 },
                                    { "--gamma", ARG_DOUBLE, 1 },
                                    { "--temperature-tau", ARG_INT, 1 },
                                    { "--gamma-tau", ARG_DOUBLE, 1 },
                                    { "--final-gamma", ARG_DOUBLE, 1 },
                                    { "--help", ARG_BOOL, 0, false }

    });
};
void outputHelp () {
    // std::cout << "Usage: " << this->argv[0] << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --tri <length> <height>          Specify a triangular lattice" << std::endl;
    std::cout << "  --qubo                           Specify that the graph is a QUBO" << std::endl;
    std::cout << "  --file <source>                  The source file path of the input" << std::endl;
    std::cout << "  --default-tri <length> <height>  Use built-in tool to create triangular lattice" << std::endl;
    std::cout << "  --gamma <gamma>                  Specify a gamma value for triangular lattice" << std::endl;
    std::cout << "  --temperature-tau <tau>          Specify a temperature tau" << std::endl;
    std::cout << "  --gamma-tau <tau>                Specify a gamma tau" << std::endl;
    std::cout << "  --final-gamma <gamma>            Specify a final gamma value" << std::endl;
    std::cout << "  --help                           Display this information" << std::endl;
    exit(0);
}
} // namespace ArgConstructor

int isArg (const char *s, const std::vector<ArgFormat>& va) {
    for (int i = 0; i < va.size(); ++i) {
        if (s == va[i].key) return i;
    }
    return -1;
}
void validInt_ (const std::string& s, const std::string errmsg = "Invalid int") {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it))
        ++it;
    if (!s.empty() && it == s.end()) return;
    ArgConstructor::outputHelp();
    throw std::invalid_argument(errmsg);
}
void validDouble_ (const std::string& s, const std::string errmsg = "Invalid double") {
    auto result = double();
    auto i = std::istringstream(s);
    i >> result;
    if (!i.fail() && i.eof()) return;
    ArgConstructor::outputHelp();
    throw std::invalid_argument(errmsg);
}
void validCount_ (const int& argc, const int& start, const int& count, const std::string errmsg = "Invalid number of arguments") {
    if (start + count < argc) return;
    ArgConstructor::outputHelp();
    throw std::invalid_argument(errmsg);
}

bool validInt (const std::string& s) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it))
        ++it;
    return !s.empty() && it == s.end();
}
bool validDouble (const std::string& s) {
    auto result = double();
    auto i = std::istringstream(s);

    i >> result;

    return !i.fail() && i.eof();
}

// argc: 4
// idx:    0 1 2 3
// args: --a 2 3 4 (start: 0 + count: 3 must < end: 4)
bool validCount (const int& argc, const int& start, const int& count) { return start + count < argc ? true : false; }

void Args::parse(const std::vector<ArgFormat>& f) {
    // Parse all the arguments
    for (int i = 0; i < argc; ++i) {
        const int& idx = isArg(argv[i], f);
        if (idx == -1) continue;
        switch (f[idx].type) {
            case ARG_INT:
                if (!validCount(argc, i, 1)) throw std::invalid_argument("Invalid int");
                if (!validInt(argv[i + 1])) throw std::invalid_argument("Invalid int");
                args.insert({ argv[i], std::atoi(argv[i + 1]) });
                break;
            case ARG_BOOL: args.insert({ argv[i], true }); break;
            case ARG_DOUBLE:
                if (!validCount(argc, i, 1)) throw std::invalid_argument("Invalid double");
                if (!validDouble(argv[i + 1])) throw std::invalid_argument("Invalid double");
                args.insert({ argv[i], atof(argv[i + 1]) });
                break;
            case ARG_STRING:
                if (!validCount(argc, i, 1)) throw std::invalid_argument("Invalid string");
                args.insert({ argv[i], argv[i + 1] });
                break;
            case ARG_VI:
                {
                    if (!validCount(argc, i, f[idx].arg_count)) throw std::invalid_argument("Invalid number of arguement count for vector<int>");
                    std::vector<int> vi;
                    for (int j = 0; j < f[idx].arg_count; ++j) {
                        if (!validInt(argv[i + j + 1])) throw std::invalid_argument("Invalid vector<int>");
                        vi.push_back(atoi(argv[i + j + 1]));
                    }
                    args.insert({ argv[i], vi });
                    break;
                }
            case ARG_VD:
                {

                    if (!validCount(argc, i, f[idx].arg_count)) throw std::invalid_argument("Invalid vector<double>");
                    std::vector<double> vd;
                    for (int j = 0; j < f[idx].arg_count; ++j) {
                        if (!validDouble(argv[i + j + 1])) throw std::invalid_argument("Invalid vector<double>");
                        vd.push_back(atof(argv[i + j + 1]));
                    }
                    args.insert({ argv[i], vd });
                    break;
                }
            case ARG_VS:
                {
                    if (!validCount(argc, i, f[idx].arg_count)) throw std::invalid_argument("Invalid vector<string>");
                    std::vector<std::string> vs;
                    for (int j = 0; j < f[idx].arg_count; ++j) {
                        vs.push_back(argv[i + j + 1]);
                    }
                    args.insert({ argv[i], vs });
                    break;
                }
            default: break;
        }
    }
    // Check if all the required arguments are present
    for (const auto& a : f) {
        if (a.required && !this->hasArg(a.key)) throw std::invalid_argument("Required argument not found");
    }
}

Args::Args(const int argc, char **argv, const std::vector<ArgFormat>& af) : argc(argc), argv(argv), args(std::unordered_map<std::string, ArgVal>()) {
    this->parse(af);
    return;
}

void Args::addConstraint(const std::vector<ArgConstraint>& arg_constraints) {
    auto exist = [&, this] (const std::string& s) -> bool { return this->args.find(s) != this->args.end(); };
    auto string_format = [] (const std::string& s1, const std::string& s2) { return s1 + " and " + s2; };
    for (const auto& ac : arg_constraints) {
        const bool& k = exist(ac.key);
        const bool& c = exist(ac.constraint);
        if (k && c && !ac.co_exist) throw std::invalid_argument("Mutual exclusion constraint violated " + string_format(ac.key, ac.constraint));
        if (k && !c && ac.co_exist) throw std::invalid_argument("Co-existence constraint violated " + string_format(ac.key, ac.constraint));
        if (!k && c && ac.co_exist) throw std::invalid_argument("Co-existence constraint violated " + string_format(ac.key, ac.constraint));
    }
    return;
}

void Args::print() const {
    for (const auto& a : args) {
        std::cout << a.first << std::endl;
        std::visit(ArgValOutput(), a.second);
    }
}

bool Args::hasArg(const std::string& s) const { return args.find(s) != args.end(); }
ArgVal Args::getArg(const std::string& s) const {
    if (args.find(s) != args.end()) return args.at(s);
    return ArgVal(false);
}
