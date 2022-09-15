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

#ifndef TRIANGLE_EDGE_COUPLE_HH
#define TRIANGLE_EDGE_COUPLE_HH

#include "TriangleEdgeModel.hh"

template <typename T> class Vector;

// Coupling length
template <typename DoubleType>
class TriangleEdgeCouple : public TriangleEdgeModel {
public:
  TriangleEdgeCouple(RegionPtr);

  void Serialize(std::ostream &) const;

private:
  TriangleEdgeCouple();
  TriangleEdgeCouple(const TriangleEdgeCouple &);
  TriangleEdgeCouple &operator=(const TriangleEdgeCouple &);
  Vector<DoubleType> calcTriangleEdgeCouple(ConstTrianglePtr) const;
  void calcTriangleEdgeScalarValues() const;
};
#endif
