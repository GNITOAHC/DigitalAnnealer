#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <fstream>
#include <iostream>
#include <map>
#include <vector>

#include "../include/Spin.h"

struct AdjNode {
    int val;
    double weight;
    AdjNode *next;
    AdjNode (int v, double w) : val(v), weight(w), next(nullptr) {}
};

class Graph {
    friend class Annealer;
    friend void testSpin(int, Graph);

  protected:
    std::vector<AdjNode *> adj_list;          // vector of pointers to AdjNode (sorted by index)
    std::map<int, std::vector<int> > adj_map; // index of node -> vector of neighbors
    std::map<int, double> constant_map;       // index of node -> constant
    std::vector<Spin> spins;                  // vector of spins (sorted by index)
    double constant;
    int length; // Length of the graph

    void privatePushBack(const int&, AdjNode *);
    void privatePushBack(const int&, const double&);

  public:
    /* Constructor */
    Graph();
    Graph(const Graph&); // Copy constructor

    /* Manipulator */
    void pushBack(const double&);                         // Push back a constant
    void pushBack(const int&, const int&, const double&); // Push back an edge
    void pushBack(const int&, const double&);             // Push back a constant_map
    void flipSpin(const int&);                            // Flip the spin of the given index
    void updateGamma(const double&);                      // Update the gamma of the graph
    void lockLength();           // Lock the length of the graph to current spins.size()
    void lockLength(const int&); // Lock the length of the graph to current spins.size()
    void growLayer(const int&, const double&); // Grow the graph by a layer

    /* Accessors */
    std::vector<Spin> getSpins() const; // Get the spin config vector of the graph
    std::vector<double> getVerticalEnergyProduct(
        const int&); // Get the vertical energy of the graph (Support for triangular lattice only)
    double getHamiltonianEnergy() const; // Get the Hamiltonian energy of the graph
    std::vector<double>
    getLayerHamiltonianEnergy() const; // Get the Hamiltonian energy of each layer
    double getHamiltonianDifference(
        const int&);       // Get the Hamiltonian difference given the indices to flip and the spin
    int getLength() const; // Get the length of the graph
    int getHeight() const; // Get the height of the graph

    /* Printer */
    void print(std::ofstream&);
    void printHLayer(std::ofstream&)
        const; // Print the Hamiltonian energy of each layer (tsv) (layer hamiltonian h_per_bit)
    void
    printConfig(std::ofstream&) const; // Print the configuration of the graph (tsv) (spin config)
};

#endif
