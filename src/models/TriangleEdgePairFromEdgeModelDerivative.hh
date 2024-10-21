/***
DEVSIM
Copyright 2020 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef TRIANGLE_EDGE_PAIR_FROM_EDGE_MODEL_DERIVATIVE_HH
#define TRIANGLE_EDGE_PAIR_FROM_EDGE_MODEL_DERIVATIVE_HH
#include "TriangleEdgeModel.hh"
#include <array>
TriangleEdgeModelPtr CreateTriangleEdgePairFromEdgeModelDerivative(
  const std::string &/*edgemodel*/,
  const std::string &/*nodemodel*/,
  RegionPtr /*rp*/
);


template <typename DoubleType>
class TriangleEdgePairFromEdgeModelDerivative : public TriangleEdgeModel {
    public:
        void Serialize(std::ostream &) const;

    private:
        friend class dsModelFactory<TriangleEdgePairFromEdgeModelDerivative>;
        TriangleEdgePairFromEdgeModelDerivative(
          const std::string &/*edgemodel*/,
          const std::string &/*nodemodel*/,
          RegionPtr /*rp*/
        );

        void derived_init();

        void calcTriangleEdgeScalarValues() const;


        const std::string edgeModelName;
        const std::string nodeModelName;
        std::array<std::string, 2> edgeModelNames;
        std::array<std::array<std::array<std::string, 2>, 2>, 3> model_names;
};
#endif

