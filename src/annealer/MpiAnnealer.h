#ifndef _MPIANNEALER_H_
#define _MPIANNEALER_H_

#include "../graph/Graph.h"
#include <functional>

typedef std::function<double(double&, double&, double&, double&)> deltaSGenFunc;
bool swap(const int, double, double, std::vector<Spin>&, deltaSGenFunc); // myrank, compare_src1, compare_src2, spin config

bool configCompare(double& src1, double& src2, double& target1, double& target2, deltaSGenFunc deltaS_func);
bool swap(const int myrank, double cmp_src1, double cmp_src2, std::vector<Spin>& config, deltaSGenFunc deltaS_func);

#endif
