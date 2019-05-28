/***
DEVSIM
Copyright 2013 Devsim LLC

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

#ifndef TRIANGLE_EDGE_SUB_MODEL_HH
#define TRIANGLE_EDGE_SUB_MODEL_HH
#include "TriangleEdgeModel.hh"
#include <string>

TriangleEdgeModelPtr CreateTriangleEdgeSubModel(const std::string &, RegionPtr, TriangleEdgeModel::DisplayType);
TriangleEdgeModelPtr CreateTriangleEdgeSubModel(const std::string &, RegionPtr, TriangleEdgeModel::DisplayType, TriangleEdgeModelPtr);

template <typename DoubleType>
class TriangleEdgeSubModel : public TriangleEdgeModel
{
    public:
        TriangleEdgeSubModel(const std::string &, RegionPtr, TriangleEdgeModel::DisplayType);
        // This model depends on this model to calculate values
        TriangleEdgeSubModel(const std::string &, RegionPtr, TriangleEdgeModel::DisplayType, ConstTriangleEdgeModelPtr);

        void Serialize(std::ostream &) const;

        static TriangleEdgeModelPtr CreateTriangleEdgeSubModel(const std::string &, RegionPtr, TriangleEdgeModel::DisplayType);
        static TriangleEdgeModelPtr CreateTriangleEdgeSubModel(const std::string &, RegionPtr, TriangleEdgeModel::DisplayType, ConstTriangleEdgeModelPtr);

    private:
        void calcTriangleEdgeScalarValues() const;
        // If we are an auxilary model, create our values from the parent
        mutable WeakConstTriangleEdgeModelPtr parentModel;
        // Detect whether parent model still exists
        mutable std::string parentModelName;
};

#endif
