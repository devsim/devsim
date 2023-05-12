/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef TETRAHEDRON_EDGE_EXPR_MODEL_HH
#define TETRAHEDRON_EDGE_EXPR_MODEL_HH
#include "TetrahedronEdgeModel.hh"
#include <string>
#include <memory>
namespace Eqo {
    class EquationObject;
    typedef std::shared_ptr<EquationObject> EqObjPtr;

}

TetrahedronEdgeModelPtr CreateTetrahedronEdgeExprModel(const std::string &, Eqo::EqObjPtr, RegionPtr, TetrahedronEdgeModel::DisplayType);

template <typename DoubleType>
class TetrahedronEdgeExprModel : public TetrahedronEdgeModel
{
    public:
        void Serialize(std::ostream &) const;

        TetrahedronEdgeExprModel(const std::string &, Eqo::EqObjPtr, RegionPtr, TetrahedronEdgeModel::DisplayType);

    private:
        void RegisterModels();
        TetrahedronEdgeExprModel();
        TetrahedronEdgeExprModel(const TetrahedronEdgeExprModel &);

        void calcTetrahedronEdgeScalarValues() const;

        const Eqo::EqObjPtr      equation;
};

#endif
