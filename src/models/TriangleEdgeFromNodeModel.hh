/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef TRIANGLE_EDGE_FROM_NODE_MODEL_HH
#define TRIANGLE_EDGE_FROM_NODE_MODEL_HH
#include "TriangleEdgeModel.hh"
TriangleEdgeModelPtr CreateTriangleEdgeFromNodeModel(const std::string &/*node0*/, const std::string &/*node1*/, const std::string &/*node2*/, const std::string &/*nodemodel*/, RegionPtr);
template <typename DoubleType>
class TriangleEdgeFromNodeModel : public TriangleEdgeModel {
    public:

        void Serialize(std::ostream &) const;

        void calcTriangleEdgeScalarValues() const;

    private:
        friend class dsModelFactory<TriangleEdgeFromNodeModel>;
        TriangleEdgeFromNodeModel(const std::string &/*node0*/, const std::string &/*node1*/, const std::string &/*node2*/, const std::string &/*nodemodel*/, RegionPtr);

        // Detect whether parent model still exists
        const std::string nodeModelName;
        const std::string edgeModel1Name;
        const std::string edgeModel2Name;
};
#endif

