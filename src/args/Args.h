#ifndef _ARGS_H_
#define _ARGS_H_

#include <set>
#include <string>
#include <tuple>

class Args {
  private:
    int argc;                // Number of arguments
    char **argv;             // Array of arguments
    int tri_length;          // Length of triangluar lattice
    int tri_height;          // Height of triangular lattice
    double tri_gamma;        // Gamma of the triangular lattice
    double tri_final_gamma;  // Final gamma of the triangular lattice
    int temperature_tau;     // Temperature tau
    int gamma_tau;           // Gamma tau
    bool use_builtin;        // Use built in function to construct triangular lattice
    bool is_qubo;            // Whether or not the graph is a QUBO
    std::string source_file; // Source file path

    /* Private parser */
    void parseArgs();
    /* Private helper */
    bool isNumber(const std::string&);
    void outputHelp() const;
    void validateInput(std::set<std::string>&) const;

  public:
    /* Constructor */
    Args(const int argc, char **argv);

    /* Getters */
    std::tuple<bool, std::pair<int, int> > getTri() const;
    std::tuple<bool, std::pair<int, int>, double> useDefault() const;
    bool isQubo() const;
    int getTemperatureTau() const;
    int getGammaTau() const;
    double getFinalGamma() const;
    std::string getSourceFile() const;
};

#endif
