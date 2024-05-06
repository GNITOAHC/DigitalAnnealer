#ifndef _SA_H_
#define _SA_H_

#include "../../annealer/Annealer.h"

struct Params_SA {
    int rank = 0;
    double init_t = 0.2;
    double final_t = 0.0;
    int tau = 1000;
};

class Anlr_SA : public Annealer {
  private:
    class Grph_SA : public Graph {
        friend class Anlr_SA;

      public:
        Grph_SA(const Graph&);
    };
    Grph_SA graph;
    Params_SA params;

  public:
    Anlr_SA(const Graph&, const Params_SA&);
    double anneal();
    void printConfig(std::ofstream&);
    double getHamiltonianEnergy() const;
};

#endif
