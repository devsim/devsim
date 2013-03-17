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

#ifndef EDGESUBMODEL_HH
#define EDGESUBMODEL_HH
#include "EdgeModel.hh"
#include <string>
// need to set general node propeties, such as positive only
// 
// This is modeled after NodeSolution
class EdgeSubModel : public EdgeModel
{
    public:
        EdgeSubModel(const std::string &, RegionPtr, EdgeModel::DisplayType);
        // This model depends on this model to calculate values
        EdgeSubModel(const std::string &, RegionPtr, EdgeModel::DisplayType, ConstEdgeModelPtr);

        static EdgeModelPtr CreateEdgeSubModel(const std::string &, RegionPtr, EdgeModel::DisplayType);
        static EdgeModelPtr CreateEdgeSubModel(const std::string &, RegionPtr, EdgeModel::DisplayType, ConstEdgeModelPtr);

        void Serialize(std::ostream &) const;

    private:

        EdgeSubModel();
        EdgeSubModel(const EdgeSubModel &);
        EdgeSubModel &operator=(const EdgeSubModel &);

        double calcSolution(ConstEdgePtr) const;
        void calcEdgeScalarValues() const;
        // If we are an auxilary model, create our values from the parent
        mutable WeakConstEdgeModelPtr parentModel;
        // Detect whether parent model still exists
        mutable std::string parentModelName;
};

#endif
