#ifndef _ANNEALER_H_
#define _ANNEALER_H_

#include <functional>

#include "../graph/Graph.h"

class Annealer {
  private:
    bool randomExec(const double, const std::function<void()>);

  public:
    int myrank;
    Annealer(const int);
    double annealTemp(std::tuple<double, double>, Graph&); // Annealing algorithm { { temp, step }, graph }
    double annealGamma(const std::tuple<double, double, double>&, Graph&,
                       const std::tuple<int, int>&); // Annealing algorithm { { gamma, step, final_gamma }, graph }
};

#endif
