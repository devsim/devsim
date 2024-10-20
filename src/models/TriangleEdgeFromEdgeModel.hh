/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef TRIANGLE_EDGE_FROM_EDGE_MODEL_HH
#define TRIANGLE_EDGE_FROM_EDGE_MODEL_HH
#include "TriangleEdgeModel.hh"
TriangleEdgeModelPtr CreateTriangleEdgeFromEdgeModel(const std::string &, RegionPtr);

template <typename DoubleType>
class TriangleEdgeFromEdgeModel : public TriangleEdgeModel {
    public:
        void Serialize(std::ostream &) const;

    private:
        friend class dsModelFactory<TriangleEdgeFromEdgeModel>;
        TriangleEdgeFromEdgeModel(const std::string &, RegionPtr);

        void derived_init();

        void calcTriangleEdgeScalarValues() const;

        // Detect whether parent model still exists
        const std::string edgeModelName;
        std::string       y_ModelName;
};
#endif
