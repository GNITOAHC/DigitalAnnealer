#ifndef _ANNEALER_H_
#define _ANNEALER_H_

#include <functional>
#include <vector>

#include "../Graph.h"

class Annealer {
  private:
    bool randomExec(const double, const std::function<void()>);
    bool mpiSwap(int, double, double, std::vector<Spin>&);
    int myrank;

  public:
    double anneal(std::tuple<double, double>, Graph&); // Annealing algorithm { { temp, step }, graph }
};

#endif
