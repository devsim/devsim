/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef TRIANGLE_EDGE_SUB_MODEL_HH
#define TRIANGLE_EDGE_SUB_MODEL_HH
#include "TriangleEdgeModel.hh"
#include <string>

TriangleEdgeModelPtr CreateTriangleEdgeSubModel(const std::string &, RegionPtr, TriangleEdgeModel::DisplayType);
TriangleEdgeModelPtr CreateTriangleEdgeSubModel(const std::string &, RegionPtr, TriangleEdgeModel::DisplayType, TriangleEdgeModelPtr);

template <typename DoubleType>
class TriangleEdgeSubModel : public TriangleEdgeModel
{
    public:
        TriangleEdgeSubModel(const std::string &, RegionPtr, TriangleEdgeModel::DisplayType);
        // This model depends on this model to calculate values
        TriangleEdgeSubModel(const std::string &, RegionPtr, TriangleEdgeModel::DisplayType, ConstTriangleEdgeModelPtr);

        void Serialize(std::ostream &) const;

        static TriangleEdgeModelPtr CreateTriangleEdgeSubModel(const std::string &, RegionPtr, TriangleEdgeModel::DisplayType);
        static TriangleEdgeModelPtr CreateTriangleEdgeSubModel(const std::string &, RegionPtr, TriangleEdgeModel::DisplayType, ConstTriangleEdgeModelPtr);

    private:
        void calcTriangleEdgeScalarValues() const;
        // If we are an auxilary model, create our values from the parent
        mutable WeakConstTriangleEdgeModelPtr parentModel;
        // Detect whether parent model still exists
        mutable std::string parentModelName;
};

#endif
