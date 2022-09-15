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

#ifndef TRIANGLE_CYLINDRICAL_EDGE_COUPLE_HH
#define TRIANGLE_CYLINDRICAL_EDGE_COUPLE_HH

#include "TriangleEdgeModel.hh"

template <typename T> class Vector;

TriangleEdgeModelPtr CreateTriangleCylindricalEdgeCouple(RegionPtr);

// Coupling length
template <typename DoubleType>
class TriangleCylindricalEdgeCouple : public TriangleEdgeModel {
public:
  void Serialize(std::ostream &) const;

  TriangleCylindricalEdgeCouple(RegionPtr);

private:
  TriangleCylindricalEdgeCouple();
  TriangleCylindricalEdgeCouple(const TriangleCylindricalEdgeCouple &);
  TriangleCylindricalEdgeCouple &
  operator=(const TriangleCylindricalEdgeCouple &);
  Vector<DoubleType>
  calcTriangleCylindricalEdgeCouple(ConstTrianglePtr,
                                    const std::string & /*RAxisVariable*/,
                                    DoubleType /*RAxis0*/) const;
  void calcTriangleEdgeScalarValues() const;
};
#endif
