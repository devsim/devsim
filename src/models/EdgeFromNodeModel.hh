/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef EDGEFROMNODEMODEL_HH
#define EDGEFROMNODEMODEL_HH
#include "EdgeModel.hh"

#include <memory>

#include <string>

EdgeModelPtr CreateEdgeFromNodeModel(const std::string &/*node 0 modelname*/, const std::string &/*node 1 modelname*/, const std::string &/*nodemodel*/, RegionPtr);

// This class relies on a parent Node Model for it's data.  It provides conversion so that a node model may be projected onto both sides of an edge.
// An edge sub model is used for node_1, if it is necessary
template <typename DoubleType>
class EdgeFromNodeModel : public EdgeModel
{
    public:
        ~EdgeFromNodeModel();

        void Serialize(std::ostream &) const;

        // This model depends on this Node model to calculate values
        EdgeFromNodeModel(const std::string &/*node 0 modelname*/, const std::string &/*node 1 modelname*/, const std::string &/*nodemodel*/, RegionPtr);

    private:

        void calcEdgeScalarValues() const;
        void setInitialValues();
        // If we are an auxilary model, create our values from the parent
        // Actually this should be by name and not by pointer reference
        // Need to do the same from EdgeSubModel, and NodeSolution
        const std::string nodeModelName;
        // This is for the second node on the edge
        mutable WeakEdgeModelPtr node1EdgeModel;
        const   std::string edgeModel1Name;
};

#endif

