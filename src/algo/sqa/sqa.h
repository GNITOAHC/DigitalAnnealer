#ifndef _SQA_H_
#define _SQA_H_

#include "../../annealer/Annealer.h"
#include <fstream>

struct Params_SQA {
    int rank = 0;
    double init_g = 0.2;
    double final_g = 0.0;
    int tau = 1000;
    double gamma = 0.2;
    int layer_count = 8;
};

class Anlr_SQA : public Annealer {
  private:
    class Grph_SQA : public Graph {
        friend class Anlr_SQA;

      public:
        Grph_SQA(const Graph&);
        void updateGamma(const double&);
        void growLayer(const int&, const double&);
    };
    Grph_SQA graph;
    Params_SQA params;

  public:
    Anlr_SQA(const Graph&, const int&); // graph, rank
    Anlr_SQA(const Graph&, const Params_SQA& params);

    void growLayer(const int&, const double&);
    void printConfig(std::ofstream&);
    double anneal();
    double getHamiltonianEnergy() const;
};

#endif
