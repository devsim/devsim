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

#ifndef TETRAHEDRON_EDGE_SUB_MODEL_HH
#define TETRAHEDRON_EDGE_SUB_MODEL_HH
#include "TetrahedronEdgeModel.hh"
#include <string>
// need to set general node propeties, such as positive only
// 
// This is modeled after NodeSolution
class TetrahedronEdgeSubModel : public TetrahedronEdgeModel
{
    public:
        TetrahedronEdgeSubModel(const std::string &, RegionPtr, TetrahedronEdgeModel::DisplayType dt);
        // This model depends on this model to calculate values
        TetrahedronEdgeSubModel(const std::string &, RegionPtr, ConstTetrahedronEdgeModelPtr, TetrahedronEdgeModel::DisplayType dt);

        void Serialize(std::ostream &) const;

        static TetrahedronEdgeModelPtr CreateTetrahedronEdgeSubModel(const std::string &, RegionPtr, TetrahedronEdgeModel::DisplayType);
        static TetrahedronEdgeModelPtr CreateTetrahedronEdgeSubModel(const std::string &, RegionPtr, TetrahedronEdgeModel::DisplayType, ConstTetrahedronEdgeModelPtr);

    private:
        double calcSolution(ConstEdgePtr) const;
        void calcTetrahedronEdgeScalarValues() const;
        // If we are an auxilary model, create our values from the parent
        mutable WeakConstTetrahedronEdgeModelPtr parentModel;
        // Detect whether parent model still exists
        mutable std::string parentModelName;
};

#endif
