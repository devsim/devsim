/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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

    private:
        friend class dsModelFactory<InterfaceNormal>;
        InterfaceNormal(const std::string &, const std::string &, const std::string &, const std::string &, const std::string &,  RegionPtr);

        void derived_init();

        void calcEdgeScalarValues() const;

        std::string interface_name;
        std::string normx_;
        std::string normy_;
        std::string normz_;
        WeakEdgeModelPtr normal_x;
        WeakEdgeModelPtr normal_y;
        WeakEdgeModelPtr normal_z;
};

#endif
