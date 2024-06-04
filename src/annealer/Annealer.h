#ifndef _ANNEALER_H_
#define _ANNEALER_H_

#include <functional>

#include "../graph/Graph.h"

class Annealer {
  protected:
    bool randomExec(const double, const std::function<void()>);

  public:
    int myrank;
    Annealer(const int);

    virtual double anneal() = 0;
};

#endif
