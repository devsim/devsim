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

#ifndef TETRAHEDRON_EDGE_FROM_EDGE_MODEL_HH
#define TETRAHEDRON_EDGE_FROM_EDGE_MODEL_HH
#include "TetrahedronEdgeModel.hh"

TetrahedronEdgeModelPtr CreateTetrahedronEdgeFromEdgeModel(const std::string &, RegionPtr);

template <typename DoubleType>
class TetrahedronEdgeFromEdgeModel : public TetrahedronEdgeModel {
    public:

        void Serialize(std::ostream &) const;

        //// Out naming convention is that the name given is the edge model
        //// The element edge model is edgemodel_ex, edgemodel_ey
        TetrahedronEdgeFromEdgeModel(const std::string &, RegionPtr);


    private:

        void calcTetrahedronEdgeScalarValues() const;

        const std::string edgeModelName;
        std::string       y_ModelName;
        std::string       z_ModelName;
};
#endif
