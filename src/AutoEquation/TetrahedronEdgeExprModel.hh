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

#ifndef TETRAHEDRON_EDGE_EXPR_MODEL_HH
#define TETRAHEDRON_EDGE_EXPR_MODEL_HH
#include "TetrahedronEdgeModel.hh"
#include <string>
#include <memory>
namespace Eqo {
    class EquationObject;
    typedef std::tr1::shared_ptr<EquationObject> EqObjPtr;

}
// need to set general node propeties, such as positive only
class TetrahedronEdgeExprModel : public TetrahedronEdgeModel
{
    public:
        TetrahedronEdgeExprModel(const std::string &, Eqo::EqObjPtr, RegionPtr, TetrahedronEdgeModel::DisplayType);

        void Serialize(std::ostream &) const;

    private:
        void RegisterModels();
        TetrahedronEdgeExprModel();
        TetrahedronEdgeExprModel(const TetrahedronEdgeExprModel &);

        void calcTetrahedronEdgeScalarValues() const;

        const Eqo::EqObjPtr      equation;
};

#endif
