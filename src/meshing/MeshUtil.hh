/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef MESH_UTIL_HH
#define MESH_UTIL_HH
#include <vector>
template <typename T>
class Vector;
class Region;
namespace MeshUtil {
extern const double tolerance;
typedef std::vector<double> pts_t;
pts_t equidistance(double xl, double xh, double sl);
pts_t variable(double xl, double xh, double sl, double sh);
///// Calls the previous two routines in order to calculate
///// meshlines
pts_t getPoints(double xl, double xh, double sl, double sh);

std::vector<Vector<double>> GetUnitVector(const Region &);

}

#endif
