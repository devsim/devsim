/***
DEVSIM
Copyright 2013 DEVSIM LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
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

