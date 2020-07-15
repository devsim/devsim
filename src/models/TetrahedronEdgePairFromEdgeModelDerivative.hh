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

#ifndef TETRAHEDRON_EDGE_PAIR_FROM_EDGE_MODEL_DERIVATIVE_HH
#define TETRAHEDRON_EDGE_PAIR_FROM_EDGE_MODEL_DERIVATIVE_HH
#include "TetrahedronEdgeModel.hh"
#include <string>
#include <array>

TetrahedronEdgeModelPtr CreateTetrahedronEdgePairFromEdgeModelDerivative(
  const std::string &/*edgemodel*/,
  const std::string &/*nodemodel*/,
  RegionPtr /*rp*/
);

template <typename DoubleType>
class TetrahedronEdgePairFromEdgeModelDerivative : public TetrahedronEdgeModel {
    public:

        void Serialize(std::ostream &) const;

        TetrahedronEdgePairFromEdgeModelDerivative(
          const std::string &/*edgemodel*/,
          const std::string &/*nodemodel*/,
          RegionPtr /*rp*/
        );

    private:
        void calcTetrahedronEdgeScalarValues() const;


        const std::string edgeModelName;
        const std::string nodeModelName;

        std::array<std::string, 2> edgeModelNames;
        std::array<std::array<std::array<std::string, 3>, 3>, 4> model_names;
};
#endif

