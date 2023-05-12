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
        //// Out naming convention is that the name given is the edge model
        //// The element edge model is edgemodel_ex, edgemodel_ey
        TriangleEdgeFromEdgeModel(const std::string &, RegionPtr);

        void Serialize(std::ostream &) const;

    private:

        void calcTriangleEdgeScalarValues() const;

        // Detect whether parent model still exists
        const std::string edgeModelName;
        std::string       y_ModelName;
};
#endif
