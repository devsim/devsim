/***
DEVSIM
Copyright 2020 Devsim LLC

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

#ifndef TETRAHEDRON_EDGE_PAIR_FROM_EDGE_MODEL_HH
#define TETRAHEDRON_EDGE_PAIR_FROM_EDGE_MODEL_HH
#include "TetrahedronEdgeModel.hh"
#include <string>
#include <array>

TetrahedronEdgeModelPtr CreateTetrahedronEdgePairFromEdgeModel(const std::string &, RegionPtr);

template <typename DoubleType>
class TetrahedronEdgePairFromEdgeModel : public TetrahedronEdgeModel {
    public:

        void Serialize(std::ostream &) const;

        TetrahedronEdgePairFromEdgeModel(const std::string &, RegionPtr);


    private:

        void calcTetrahedronEdgeScalarValues() const;

        const std::string edgeModelName;
        std::array<std::array<std::string, 3>, 2> model_names;
};
#endif

