#ifndef _TRI_H_
#define _TRI_H_

#include "../Graph.h"

#include <vector>

// Squared order parameter
std::vector<double> getSquaredOP(const Graph& graph);
// Make a triangular lattice graph ( length, height, gamma )
// Graph makeGraph(const int&, const int&, const double&);

#endif
