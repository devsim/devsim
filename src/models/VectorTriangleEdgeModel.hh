/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef VECTOR_TRIANGLE_EDGE_FROM_EDGE_MODEL_HH
#define VECTOR_TRIANGLE_EDGE_FROM_EDGE_MODEL_HH
#include "TriangleEdgeModel.hh"

TriangleEdgeModelPtr CreateVectorTriangleEdgeModel(const std::string &, RegionPtr);

template <typename DoubleType>
class VectorTriangleEdgeModel : public TriangleEdgeModel {
    public:

        void Serialize(std::ostream &) const;

        //// Out naming convention is that the name given is the edge model
        //// The element edge model is edgemodel_ex, edgemodel_ey
        VectorTriangleEdgeModel(const std::string &, RegionPtr);

    private:

        void calcTriangleEdgeScalarValues() const;

        // Detect whether parent model still exists
        const std::string elementEdgeModelName;
        std::string       y_ModelName;
};
#endif
