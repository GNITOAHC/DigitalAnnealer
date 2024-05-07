#ifndef _SA_H_
#define _SA_H_

#include "../../annealer/Annealer.h"

struct Params_SA {
    int rank = 0;
    double init_t = 2.0;
    double final_t = 0.0;
    int tau = 1000;
};

class Anlr_SA : public Annealer {
  private:
    class Grph_SA : public Graph {
        friend class Anlr_SA;

      public:
        Grph_SA();
        Grph_SA(const Graph&);
    };
    Grph_SA graph;
    Params_SA params;

  public:
    Anlr_SA();
    Anlr_SA(const Graph&, const Params_SA&);
    Params_SA getParams() const;

    // Virtual functions
    double anneal();

    // Reexported functions from Graph
    int getLength() const;
    int getHeight() const;
    std::vector<Spin> getSpins() const;

    Grph_SA getGraph () const { return this->graph; }

    double getHamiltonianEnergy() const;

    // Printer
    void printHLayer(std::ofstream&) const;
    void printConfig(std::ofstream&) const;
};

#endif
