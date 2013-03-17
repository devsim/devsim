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

#ifndef TRIANGLE_EDGE_EXPR_MODEL_HH
#define TRIANGLE_EDGE_EXPR_MODEL_HH
#include "TriangleEdgeModel.hh"
#include <string>
#include <memory>
namespace Eqo {
    class EquationObject;
    typedef std::tr1::shared_ptr<EquationObject> EqObjPtr;

}
// need to set general node propeties, such as positive only
class TriangleEdgeExprModel : public TriangleEdgeModel
{
    public:
        TriangleEdgeExprModel(const std::string &, Eqo::EqObjPtr, RegionPtr, TriangleEdgeModel::DisplayType);

        void Serialize(std::ostream &) const;

    private:
        void RegisterModels();
        TriangleEdgeExprModel();
        TriangleEdgeExprModel(const TriangleEdgeExprModel &);

        void calcTriangleEdgeScalarValues() const;

        const Eqo::EqObjPtr      equation;
};

#endif
