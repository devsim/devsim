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
        void Serialize(std::ostream &) const;

    private:
        friend class dsModelFactory<TriangleEdgePairFromEdgeModel>;
        TriangleEdgePairFromEdgeModel(const std::string &, RegionPtr);

        void derived_init();

        void calcTriangleEdgeScalarValues() const;

        // Detect whether parent model still exists
        const std::string edgeModelName;
        std::array<std::array<std::string, 2>, 2> model_names;
};
#endif

