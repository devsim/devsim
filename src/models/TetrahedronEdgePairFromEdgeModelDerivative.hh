/***
DEVSIM
Copyright 2020 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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

