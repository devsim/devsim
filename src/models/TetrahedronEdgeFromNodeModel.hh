/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef TETRAHEDRON_EDGE_FROM_NODE_MODEL_HH
#define TETRAHEDRON_EDGE_FROM_NODE_MODEL_HH
#include "TetrahedronEdgeModel.hh"
TetrahedronEdgeModelPtr CreateTetrahedronEdgeFromNodeModel(const std::string &/*en0*/, const std::string &/*en1*/, const std::string &/*en2*/, const std::string &/*en3*/, const std::string &/*nodemodel*/, RegionPtr);

template <typename DoubleType>
class TetrahedronEdgeFromNodeModel : public TetrahedronEdgeModel {
    public:
        void Serialize(std::ostream &) const;

        //// Out naming convention is that the name given is the edge model
        TetrahedronEdgeFromNodeModel(const std::string &/*en0*/, const std::string &/*en1*/, const std::string &/*en2*/, const std::string &/*en3*/, const std::string &/*nodemodel*/, RegionPtr);

        void calcTetrahedronEdgeScalarValues() const;

    private:

        // Detect whether parent model still exists
        const std::string nodeModelName;
        const std::string edgeModel1Name;
        const std::string edgeModel2Name;
        const std::string edgeModel3Name;
};
#endif
