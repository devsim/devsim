/***
DEVSIM
Copyright 2020 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef TRIANGLE_EDGE_PAIR_FROM_EDGE_MODEL_HH
#define TRIANGLE_EDGE_PAIR_FROM_EDGE_MODEL_HH
#include "TriangleEdgeModel.hh"
#include <array>
TriangleEdgeModelPtr CreateTriangleEdgePairFromEdgeModel(const std::string &, RegionPtr);

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

