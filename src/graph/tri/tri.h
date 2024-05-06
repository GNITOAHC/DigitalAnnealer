#ifndef _TRI_H_
#define _TRI_H_

#include "../Graph.h"

#include <vector>

namespace tri {

std::vector<double> getSquaredOP(const Graph& graph); // Squared order parameter
std::vector<double> getSquaredOP(const std::vector<Spin>&, const int); // graph.spins, graph.length
Graph makeGraph(const int&);                          // makeGraph(length)
void printTriConf(const Graph&, std::ofstream&);      // printTriConf(graph)

} // namespace tri

#endif
