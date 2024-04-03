#ifndef _TRI_H_
#define _TRI_H_

#include "../Graph.h"

#include <vector>

namespace tri {
// Squared order parameter
std::vector<double> getSquaredOP(const Graph& graph);
Graph makeGraph(const int&); // makeGraph(length)
} // namespace tri
// Make a triangular lattice graph ( length, height, gamma )
// Graph makeGraph(const int&, const int&, const double&);

#endif
