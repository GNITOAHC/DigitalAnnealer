#ifndef _ANNEALER_H_
#define _ANNEALER_H_

#include <functional>

#include "Graph.h"

class Annealer {
  private:
    bool randomExec(const double, const std::function<void()>);

  public:
    double anneal(std::tuple<double, double>, Graph&); // Annealing algorithm { { temp, step }, graph }
};

#endif
