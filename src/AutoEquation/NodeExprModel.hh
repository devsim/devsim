/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef NODEEXPRMODEL_HH
#define NODEEXPRMODEL_HH
#include "NodeModel.hh"
#include <string>
#include <memory>
namespace Eqo {
    class EquationObject;
    typedef std::shared_ptr<EquationObject> EqObjPtr;

}

NodeModelPtr CreateNodeExprModel(const std::string &, Eqo::EqObjPtr, RegionPtr, NodeModel::DisplayType, ContactPtr cp);

template <typename DoubleType>
class NodeExprModel : public NodeModel
{
    public:

        void Serialize(std::ostream &) const;

        NodeExprModel(const std::string &, Eqo::EqObjPtr, RegionPtr, NodeModel::DisplayType, ContactPtr cp = nullptr);

    private:

        void RegisterModels();
        NodeExprModel();
        NodeExprModel(const NodeExprModel &);

        void calcNodeScalarValues() const;
        void setInitialValues();
        const Eqo::EqObjPtr      equation;
};

#endif

