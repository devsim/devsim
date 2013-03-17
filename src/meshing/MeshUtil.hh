/***
DEVSIM
Copyright 2013 Devsim LLC

This file is part of DEVSIM.

DEVSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, version 3.

DEVSIM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with DEVSIM.  If not, see <http://www.gnu.org/licenses/>.
***/

#ifndef MESH_UTIL_HH
#define MESH_UTIL_HH
#include <vector>
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

std::vector<Vector> GetUnitVector(const Region &);

}

#endif
