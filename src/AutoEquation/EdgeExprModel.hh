/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef EDGEEXPRMODEL_HH
#define EDGEEXPRMODEL_HH
#include "EdgeModel.hh"
#include <string>
#include <memory>
namespace Eqo {
    class EquationObject;
    typedef std::shared_ptr<EquationObject> EqObjPtr;

}

EdgeModelPtr CreateEdgeExprModel(const std::string &, Eqo::EqObjPtr, RegionPtr, EdgeModel::DisplayType, ContactPtr);

template <typename DoubleType>
class EdgeExprModel : public EdgeModel
{
    public:
        void Serialize(std::ostream &) const;

        EdgeExprModel(const std::string &, Eqo::EqObjPtr, RegionPtr, EdgeModel::DisplayType, ContactPtr cp = nullptr);

    private:
        void RegisterModels();
        EdgeExprModel();
        EdgeExprModel(const EdgeExprModel &);

        void calcEdgeScalarValues() const;

        const Eqo::EqObjPtr      equation;
};

#endif
