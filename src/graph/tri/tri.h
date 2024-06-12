#ifndef _TRI_H_
#define _TRI_H_

#include "../Graph.h"

#include <vector>

namespace tri {

std::vector<double> getSquaredOP(const std::vector<Spin>&, const int);   // getSquaredOP(graph.spins, graph.length)
Graph makeGraph(const int&);                                             // makeGraph(length)
void printTriConf(const std::vector<Spin>&, const int&, std::ofstream&); // printTriConf(graph.spins, length, output_stream)

} // namespace tri

#endif
