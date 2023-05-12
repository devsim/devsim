/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef EDGESUBMODEL_HH
#define EDGESUBMODEL_HH
#include "EdgeModel.hh"
#include <string>

EdgeModelPtr CreateEdgeSubModel(const std::string &, RegionPtr, EdgeModel::DisplayType);
EdgeModelPtr CreateEdgeSubModel(const std::string &, RegionPtr, EdgeModel::DisplayType, EdgeModelPtr);

template <typename DoubleType>
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

        void calcEdgeScalarValues() const;
        // If we are an auxilary model, create our values from the parent
        mutable WeakConstEdgeModelPtr parentModel;
        // Detect whether parent model still exists
        mutable std::string parentModelName;
};
#endif

