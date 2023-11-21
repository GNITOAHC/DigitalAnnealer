#ifndef _ARGS_H_
#define _ARGS_H_

#include <string>
#include <tuple>

class Args {
  private:
    int argc;                // Number of arguments
    char **argv;             // Array of arguments
    int tri_length;          // Length of triangluar lattice
    int tri_height;          // Height of triangular lattice
    bool is_qubo;            // Whether or not the graph is a QUBO
    std::string source_file; // Source file path

    /* Private parser */
    void parseArgs();
    /* Private helper */
    bool isNumber(const std::string&);

  public:
    /* Constructor */
    Args(const int argc, char **argv);

    /* Getters */
    std::tuple<bool, std::pair<int, int> > getTri() const;
    bool isQubo() const;
    std::string getSourceFile() const;
};

#endif
