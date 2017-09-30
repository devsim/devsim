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

#ifndef INTERFACENORMAL_HH
#define INTERFACENORMAL_HH
#include "EdgeModel.hh"
#include <string>

class Node;
typedef Node *NodePtr;
typedef const Node *ConstNodePtr;

class Edge;
typedef Edge *EdgePtr;
typedef const Edge *ConstEdgePtr;

EdgeModelPtr CreateInterfaceNormal(const std::string &, const std::string &, const std::string &, const std::string &, const std::string &,  RegionPtr);

template <typename DoubleType>
class InterfaceNormal : public EdgeModel
{
    public:

        void Serialize(std::ostream &) const;

        InterfaceNormal(const std::string &, const std::string &, const std::string &, const std::string &, const std::string &,  RegionPtr);

    private:
        void calcEdgeScalarValues() const;

        std::string interface_name;
        WeakEdgeModelPtr normal_x;
        WeakEdgeModelPtr normal_y;
        WeakEdgeModelPtr normal_z;
};

#endif
