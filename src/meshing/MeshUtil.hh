/***
DEVSIM
Copyright 2013 Devsim LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
***/

#ifndef MESH_UTIL_HH
#define MESH_UTIL_HH
#include <vector>
template <typename T> class Vector;
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

} // namespace MeshUtil

#endif
