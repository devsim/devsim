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

#ifndef TETRAHEDRON_NODE_VOLUME_HH
#define TETRAHEDRON_NODE_VOLUME_HH
#include "TetrahedronEdgeModel.hh"

class Node;
typedef Node *NodePtr;
typedef const Node *ConstNodePtr;

class Tetrahedron;
typedef Tetrahedron *TetrahedronPtr;
typedef const Tetrahedron *ConstTetrahedronPtr;

template <typename DoubleType>
class TetrahedronNodeVolume : public TetrahedronEdgeModel
{
    public:
        TetrahedronNodeVolume(RegionPtr);

        void Serialize(std::ostream &) const;

    private:
        void   calcTetrahedronEdgeScalarValues() const;
        void   setInitialValues();
};

#endif

