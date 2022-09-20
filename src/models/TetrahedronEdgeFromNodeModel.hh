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

#ifndef TETRAHEDRON_EDGE_FROM_NODE_MODEL_HH
#define TETRAHEDRON_EDGE_FROM_NODE_MODEL_HH
#include "TetrahedronEdgeModel.hh"
TetrahedronEdgeModelPtr CreateTetrahedronEdgeFromNodeModel(const std::string &/*en0*/, const std::string &/*en1*/, const std::string &/*en2*/, const std::string &/*en3*/, const std::string &/*nodemodel*/, RegionPtr);

template <typename DoubleType>
class TetrahedronEdgeFromNodeModel : public TetrahedronEdgeModel {
    public:
        void Serialize(std::ostream &) const;

        //// Out naming convention is that the name given is the edge model
        TetrahedronEdgeFromNodeModel(const std::string &/*en0*/, const std::string &/*en1*/, const std::string &/*en2*/, const std::string &/*en3*/, const std::string &/*nodemodel*/, RegionPtr);

        void calcTetrahedronEdgeScalarValues() const;

    private:

        // Detect whether parent model still exists
        const std::string nodeModelName;
        const std::string edgeModel1Name;
        const std::string edgeModel2Name;
        const std::string edgeModel3Name;
};
#endif
