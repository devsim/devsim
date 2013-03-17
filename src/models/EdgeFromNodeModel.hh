/***
DEVSIM
Copyright 2013 Devsim LLC

This file is part of DEVSIM.

DEVSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, version 3.

DEVSIM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with DEVSIM.  If not, see <http://www.gnu.org/licenses/>.
***/

#ifndef EDGEFROMNODEMODEL_HH
#define EDGEFROMNODEMODEL_HH
#include "EdgeModel.hh"

#include "dsmemory.hh"

class NodeModel;
typedef std::tr1::weak_ptr<NodeModel>         WeakNodeModelPtr;
typedef std::tr1::shared_ptr<NodeModel>       NodeModelPtr;
typedef std::tr1::shared_ptr<const NodeModel> ConstNodeModelPtr;

#include <string>
// This class relies on a parent Node Model for it's data.  It provides conversion so that a node model may be projected onto both sides of an edge.
// An edge sub model is used for node_1, if it is necessary
class EdgeFromNodeModel : public EdgeModel
{
    public:
        // This model depends on this Node model to calculate values
        EdgeFromNodeModel(const std::string &/*node 0 modelname*/, const std::string &/*node 1 modelname*/, const std::string &/*nodemodel*/, RegionPtr);
        ~EdgeFromNodeModel();

        void Serialize(std::ostream &) const;

    private:
        double calcSolution(ConstEdgePtr) const;
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

