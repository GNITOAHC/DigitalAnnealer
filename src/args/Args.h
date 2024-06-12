#ifndef _ARGS_H_
#define _ARGS_H_

#include "../../lib/ArgParse/ArgParse.h"
#include "../include/AnnealFunc.h"
#include <vector>

class CustomArgs : public argparse::Args {
  public:
    // using argparse::Args::Args;
    CustomArgs(const int, char **);
    CustomArgs(const int, char **, const std::vector<argparse::ArgFormat>&);
    ANNEAL_FUNC getStrategy() const;

  private:
    std::vector<struct argparse::ArgFormat> argsConstruct() const;
    std::vector<struct argparse::ArgConstraint> argsConstraint() const;
    void customConstraintsCheck() const;
    void outputHelp() const;
};

#endif
