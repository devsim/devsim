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

#ifndef TRIANGLE_EDGE_SUB_MODEL_HH
#define TRIANGLE_EDGE_SUB_MODEL_HH
#include "TriangleEdgeModel.hh"
#include <string>
// need to set general node propeties, such as positive only
// 
// This is modeled after NodeSolution
class TriangleEdgeSubModel : public TriangleEdgeModel
{
    public:
        TriangleEdgeSubModel(const std::string &, RegionPtr, TriangleEdgeModel::DisplayType);
        // This model depends on this model to calculate values
        TriangleEdgeSubModel(const std::string &, RegionPtr, ConstTriangleEdgeModelPtr, TriangleEdgeModel::DisplayType);

        void Serialize(std::ostream &) const;

        static TriangleEdgeModelPtr CreateTriangleEdgeSubModel(const std::string &, RegionPtr, TriangleEdgeModel::DisplayType);
        static TriangleEdgeModelPtr CreateTriangleEdgeSubModel(const std::string &, RegionPtr, TriangleEdgeModel::DisplayType, ConstTriangleEdgeModelPtr);

    private:
        double calcSolution(ConstEdgePtr) const;
        void calcTriangleEdgeScalarValues() const;
        // If we are an auxilary model, create our values from the parent
        mutable WeakConstTriangleEdgeModelPtr parentModel;
        // Detect whether parent model still exists
        mutable std::string parentModelName;
};

#endif
