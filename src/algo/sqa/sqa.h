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
        Grph_SQA();
        Grph_SQA(const Graph&);
        void updateGamma(const double&);
        void growLayer(const int&, const double&);
    };
    Grph_SQA graph;
    Params_SQA params;

  public:
    Anlr_SQA();
    Anlr_SQA(const Graph&, const int&); // graph, rank
    Anlr_SQA(const Graph&, const Params_SQA& params);
    Params_SQA getParams() const;

    // Virtual functions
    double anneal();

    // Reexported functions from Graph
    int getLength() const;
    int getHeight() const;
    std::vector<Spin> getSpins() const;

    Grph_SQA getGraph () const { return this->graph; }

    void growLayer(const int&, const double&);
    double getHamiltonianEnergy() const;

    // Printer
    void printHLayer(std::ofstream&) const;
    void printConfig(std::ofstream&) const;
};

#endif
