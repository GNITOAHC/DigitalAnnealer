#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <iostream>
#include <map>
#include <vector>

#include "Spin.h"

struct AdjNode {
    int val;
    double weight;
    AdjNode *next;
    AdjNode(int v, double w) : val(v), weight(w), next(nullptr) {}
};

class Graph {
    friend class Annealer;
    friend void testSpin(int, Graph);

  private:
    std::vector<AdjNode *> adj_list;
    std::map<int, std::vector<int> > adj_map;
    std::vector<Spin> spins;

    void privatePushBack(const int&, AdjNode *);

  public:
    /* Manipulator */
    void pushBack(const int&, const int&, const double&); // Push back an edge
    void flipSpin(const int&);                            // Flip the spin of the given index

    /* Accessors */
    double getOrderParameterLengthSquared();     // Get the order parameter length squared
    double getHamiltonianEnergy();               // Get the Hamiltonian energy of the graph
    double getHamiltonianDifference(const int&); // Get the Hamiltonian difference given the indices to flip and the spin

    /* Printer */
    void print();
};

#endif
