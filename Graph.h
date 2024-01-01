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
    std::vector<AdjNode *> adj_list;          // vector of pointers to AdjNode (sorted by index)
    std::map<int, std::vector<int> > adj_map; // index of node -> vector of neighbors
    std::map<int, double> constant_map;       // index of node -> constant
    std::vector<Spin> spins;                  // vector of spins (sorted by index)
    double constant;

    void privatePushBack(const int&, AdjNode *);
    void privatePushBack(const int&, const double&);

  public:
    /* Constructor */
    Graph();

    /* Manipulator */
    void pushBack(const double&);                            // Push back a constant
    void pushBack(const int&, const int&, const double&);    // Push back an edge
    void pushBack(const int&, const double&);                // Push back a constant_map
    void flipSpin(const int&);                               // Flip the spin of the given index
    void updateGamma(const double&, const int&, const int&); // Update the gamma of the graph

    /* Accessors */
    std::vector<Spin> getSpins() const; // Get the spin config vector of the graph
    std::vector<double> getOrderParameterLengthSquared(const int&,
                                                       const int&); // Get the order parameter length squared (Support for triangular lattice only)
    double getHamiltonianEnergy();                                  // Get the Hamiltonian energy of the graph
    std::vector<double> getLayerHamiltonianEnergy(const int&);      // Get the Hamiltonian energy of each layer (Support for triangular lattice only)
    double getHamiltonianDifference(const int&);                    // Get the Hamiltonian difference given the indices to flip and the spin

    /* Printer */
    void print();
};

#endif
