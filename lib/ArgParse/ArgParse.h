#ifndef _ARGPARSE_LIB_H_
#define _ARGPARSE_LIB_H_

#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

// using ArgKey = std::string;
// using ArgVal = std::variant<int, bool, double, std::string, std::vector<int>, std::vector<double>, std::vector<std::string> >;
typedef std::string ArgKey;
typedef std::variant<int, bool, double, std::string, std::vector<int>, std::vector<double>, std::vector<std::string> > ArgVal;

namespace argparse {

struct ArgValOutput {
    void operator()(const int& i) { std::cout << i << std::endl; }
    void operator()(const bool& b) { std::cout << b << std::endl; }
    void operator()(const double& d) { std::cout << d << std::endl; }
    void operator()(const std::string& s) { std::cout << s << std::endl; }
    void operator()(const std::vector<int>& vi) {
        for (const auto& i : vi)
            std::cout << i << std::endl;
    }
    void operator()(const std::vector<double>& vd) {
        for (const auto& d : vd)
            std::cout << d << std::endl;
    }
    void operator()(const std::vector<std::string>& vs) {
        for (const auto& s : vs)
            std::cout << s << std::endl;
    }
};

enum ArgType { NONE, ARG_INT, ARG_BOOL, ARG_DOUBLE, ARG_STRING, ARG_VI, ARG_VD, ARG_VS };
struct ArgFormat {
    std::string key;
    ArgType type;
    int arg_count;
    bool required;
    ArgFormat(const std::string& s, const ArgType& t, const int& c) : key(s), type(t), arg_count(c), required(false) {}
    ArgFormat(const std::string& s, const ArgType& t, const int& c, const bool& r) : key(s), type(t), arg_count(c), required(r) {}
};
enum ArgConstraintType { COEXIST, MUTEX, REQUIRE };
struct ArgConstraint {
    std::string key;
    std::string constraint;
    ArgConstraintType type; // If false, the constraint is a mutual exclusion
    ArgConstraint(const std::string& k, const std::string& c) : key(k), constraint(c), type(COEXIST) {}
    ArgConstraint(const std::string& k, const std::string& c, const ArgConstraintType& act) : key(k), constraint(c), type(act) {}
};

namespace ArgConstructor {
std::vector<ArgFormat> argsConstruct();
}

class Args {
  private:
    int argc;
    char **argv;
    std::unordered_map<ArgKey, ArgVal> args;

    void parse(const std::vector<ArgFormat>&);

  public:
    Args(const int argc, char **argv, const std::vector<ArgFormat>& af);
    void addConstraint(const std::vector<ArgConstraint>&);
    void print() const;
    bool hasArg(const std::string&) const;
    ArgVal getArg(const std::string&) const;
};

} // namespace argparse

#endif
