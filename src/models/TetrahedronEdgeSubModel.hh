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

#ifndef TETRAHEDRON_EDGE_SUB_MODEL_HH
#define TETRAHEDRON_EDGE_SUB_MODEL_HH
#include "TetrahedronEdgeModel.hh"
#include <string>

TetrahedronEdgeModelPtr CreateTetrahedronEdgeSubModel(const std::string &, RegionPtr, TetrahedronEdgeModel::DisplayType);
TetrahedronEdgeModelPtr CreateTetrahedronEdgeSubModel(const std::string &, RegionPtr, TetrahedronEdgeModel::DisplayType, TetrahedronEdgeModelPtr);

template <typename DoubleType>
class TetrahedronEdgeSubModel : public TetrahedronEdgeModel
{
    public:
        TetrahedronEdgeSubModel(const std::string &, RegionPtr, TetrahedronEdgeModel::DisplayType dt);
        // This model depends on this model to calculate values
        TetrahedronEdgeSubModel(const std::string &, RegionPtr, TetrahedronEdgeModel::DisplayType dt, ConstTetrahedronEdgeModelPtr);

        void Serialize(std::ostream &) const;

        static TetrahedronEdgeModelPtr CreateTetrahedronEdgeSubModel(const std::string &, RegionPtr, TetrahedronEdgeModel::DisplayType);
        static TetrahedronEdgeModelPtr CreateTetrahedronEdgeSubModel(const std::string &, RegionPtr, TetrahedronEdgeModel::DisplayType, ConstTetrahedronEdgeModelPtr);

    private:
        void calcTetrahedronEdgeScalarValues() const;
        // If we are an auxilary model, create our values from the parent
        mutable WeakConstTetrahedronEdgeModelPtr parentModel;
        // Detect whether parent model still exists
        mutable std::string parentModelName;
};

#endif
