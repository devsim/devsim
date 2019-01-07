/***
DEVSIM
Copyright 2013 Devsim LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
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

