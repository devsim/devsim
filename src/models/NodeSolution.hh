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

#ifndef NODESOLUTION_HH
#define NODESOLUTION_HH
#include "NodeModel.hh"
#include <string>
// need to set general node propeties, such as positive only
class NodeSolution : public NodeModel
{
    public:
        NodeSolution(const std::string &, RegionPtr);
        // This model depends on this model to calculate values
        NodeSolution(const std::string &, RegionPtr, NodeModelPtr);

        static NodeModelPtr CreateNodeSolution(const std::string &, RegionPtr);
        static NodeModelPtr CreateNodeSolution(const std::string &, RegionPtr, NodeModelPtr);

        void Serialize(std::ostream &) const;

    private:
        double calcSolution(ConstNodePtr) const;
        void calcNodeScalarValues() const;
        void setInitialValues();
        // If we are an auxilary model, create our values from the parent
        mutable WeakConstNodeModelPtr parentModel;
        // Detect whether parent model still exists
        mutable std::string  parentModelName;
};

#endif
