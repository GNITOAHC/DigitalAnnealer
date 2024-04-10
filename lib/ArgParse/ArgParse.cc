#include "./ArgParse.h"

#include <iostream>
#include <sstream>

int isArg (const char *s, const std::vector<argparse::ArgFormat>& va) {
    for (int i = 0; i < va.size(); ++i) {
        if (s == va[i].key) return i;
    }
    return -1;
}
void validateInt (const std::string& s, const std::string errmsg = "Invalid int") {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it))
        ++it;
    if (!s.empty() && it == s.end()) return;
    throw std::invalid_argument(errmsg);
}
void validateDouble (const std::string& s, const std::string errmsg = "Invalid double") {
    auto result = double();
    auto i = std::istringstream(s);
    i >> result;
    if (!i.fail() && i.eof()) return;
    throw std::invalid_argument(errmsg);
}
// argc: 4
// idx:    0 1 2 3
// args: --a 2 3 4 (start: 0 + count: 3 must < end: 4)
void validateCount (const int& argc, const int& start, const int& count, const std::string errmsg = "Invalid number of arguments") {
    if (start + count < argc) return;
    throw std::invalid_argument(errmsg);
}

namespace argparse {
void Args::parse(const std::vector<ArgFormat>& f) {
    // Parse all the arguments
    for (int i = 0; i < argc; ++i) {
        const int& idx = isArg(argv[i], f);
        if (idx == -1) continue;
        switch (f[idx].type) {
            case ARG_INT:
                validateCount(argc, i, 1);
                validateInt(argv[i + 1], "Invalid int on argument " + f[idx].key);
                args.insert({ argv[i], std::atoi(argv[i + 1]) });
                break;
            case ARG_BOOL: args.insert({ argv[i], true }); break;
            case ARG_DOUBLE:
                validateCount(argc, i, 1);
                validateDouble(argv[i + 1], "Invalid double on argument " + f[idx].key);
                args.insert({ argv[i], atof(argv[i + 1]) });
                break;
            case ARG_STRING:
                validateCount(argc, i, 1);
                args.insert({ argv[i], argv[i + 1] });
                break;
            case ARG_VI:
                {
                    validateCount(argc, i, f[idx].arg_count);
                    std::vector<int> vi;
                    for (int j = 0; j < f[idx].arg_count; ++j) {
                        validateInt(argv[i + j + 1], "Invalid vector<int> on argument " + f[idx].key);
                        vi.push_back(atoi(argv[i + j + 1]));
                    }
                    args.insert({ argv[i], vi });
                    break;
                }
            case ARG_VD:
                {
                    validateCount(argc, i, f[idx].arg_count);
                    std::vector<double> vd;
                    for (int j = 0; j < f[idx].arg_count; ++j) {
                        validateDouble(argv[i + j + 1], "Invalid vector<double> on argument " + f[idx].key);
                        vd.push_back(atof(argv[i + j + 1]));
                    }
                    args.insert({ argv[i], vd });
                    break;
                }
            case ARG_VS:
                {
                    validateCount(argc, i, f[idx].arg_count);
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
        if (k && c && ac.type == MUTEX) throw std::invalid_argument("Mutual exclusion constraint violated " + string_format(ac.key, ac.constraint));
        if (k && !c && ac.type == COEXIST) throw std::invalid_argument("Co-existence constraint violated " + string_format(ac.key, ac.constraint));
        if (!k && c && ac.type == COEXIST) throw std::invalid_argument("Co-existence constraint violated " + string_format(ac.key, ac.constraint));
        if (k && !c && ac.type == REQUIRE) throw std::invalid_argument("Required constraint violated" + string_format(ac.key, ac.constraint));
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
} // namespace argparse
