/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef TRIANGLE_EDGE_EXPR_MODEL_HH
#define TRIANGLE_EDGE_EXPR_MODEL_HH
#include "TriangleEdgeModel.hh"
#include <string>
#include <memory>
namespace Eqo {
    class EquationObject;
    typedef std::shared_ptr<EquationObject> EqObjPtr;

}

TriangleEdgeModelPtr CreateTriangleEdgeExprModel(const std::string &, Eqo::EqObjPtr, RegionPtr, TriangleEdgeModel::DisplayType);

template <typename DoubleType>
class TriangleEdgeExprModel : public TriangleEdgeModel
{
    public:

        void Serialize(std::ostream &) const;

        TriangleEdgeExprModel(const std::string &, Eqo::EqObjPtr, RegionPtr, TriangleEdgeModel::DisplayType);

    private:

        void RegisterModels();
        TriangleEdgeExprModel();
        TriangleEdgeExprModel(const TriangleEdgeExprModel &);

        void calcTriangleEdgeScalarValues() const;

        const Eqo::EqObjPtr      equation;
};

#endif
