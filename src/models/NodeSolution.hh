/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef NODESOLUTION_HH
#define NODESOLUTION_HH
#include "NodeModel.hh"
#include <string>

NodeModelPtr CreateNodeSolution(const std::string &, RegionPtr, NodeModel::DisplayType);
NodeModelPtr CreateNodeSolution(const std::string &, RegionPtr, NodeModel::DisplayType, NodeModelPtr);

template <typename DoubleType>
class NodeSolution : public NodeModel
{
    public:

        void Serialize(std::ostream &) const;

        NodeSolution(const std::string &, RegionPtr, NodeModel::DisplayType);
        // This model depends on this model to calculate values
        NodeSolution(const std::string &, RegionPtr, NodeModel::DisplayType, NodeModelPtr);

    private:

        void calcNodeScalarValues() const;
        void setInitialValues();
        // If we are an auxilary model, create our values from the parent
        mutable WeakConstNodeModelPtr parentModel;
        // Detect whether parent model still exists
        mutable std::string  parentModelName;
};

#endif

