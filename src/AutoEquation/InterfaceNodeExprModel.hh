/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef INTERFACENODEEXPRMODEL_HH
#define INTERFACENODEEXPRMODEL_HH
#include "InterfaceNodeModel.hh"
#include <string>
#include <memory>
namespace Eqo {
    class EquationObject;
    typedef std::shared_ptr<EquationObject> EqObjPtr;

}

InterfaceNodeModelPtr CreateInterfaceNodeExprModel(const std::string &, Eqo::EqObjPtr, InterfacePtr);

// need to set general node properties, such as positive only
template <typename DoubleType>
class InterfaceNodeExprModel : public InterfaceNodeModel
{
    public:

        void Serialize(std::ostream &) const;

        InterfaceNodeExprModel(const std::string &, Eqo::EqObjPtr, InterfacePtr);

    private:

        void calcNodeScalarValues() const;
        void RegisterModels();

        InterfaceNodeExprModel();
        InterfaceNodeExprModel(const InterfaceNodeExprModel &);

        const Eqo::EqObjPtr      equation;
};

#endif

