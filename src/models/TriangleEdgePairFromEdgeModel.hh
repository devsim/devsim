/***
DEVSIM
Copyright 2020 Devsim LLC

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

#ifndef TRIANGLE_EDGE_PAIR_FROM_EDGE_MODEL_HH
#define TRIANGLE_EDGE_PAIR_FROM_EDGE_MODEL_HH
#include "TriangleEdgeModel.hh"
#include <array>
TriangleEdgeModelPtr CreateTriangleEdgePairFromEdgeModel(const std::string &,
                                                         RegionPtr);

template <typename DoubleType>
class TriangleEdgePairFromEdgeModel : public TriangleEdgeModel {
public:
  //// Out naming convention is that the name given is the edge model
  //// The element edge model is edgemodel_ex, edgemodel_ey
  TriangleEdgePairFromEdgeModel(const std::string &, RegionPtr);

  void Serialize(std::ostream &) const;

private:
  void calcTriangleEdgeScalarValues() const;

  // Detect whether parent model still exists
  const std::string edgeModelName;
  std::array<std::array<std::string, 2>, 2> model_names;
};
#endif
