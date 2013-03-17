/***
DEVSIM
Copyright 2013 Devsim LLC

This file is part of DEVSIM.

DEVSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, version 3.

DEVSIM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with DEVSIM.  If not, see <http://www.gnu.org/licenses/>.
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

class InterfaceNormal : public EdgeModel
{
    public:
        InterfaceNormal(const std::string &, const std::string &, const std::string &, const std::string &, const std::string &,  RegionPtr);

        void Serialize(std::ostream &) const;

    private:
        void calcEdgeScalarValues() const;

        std::string interface_name;
        WeakEdgeModelPtr normal_x;
        WeakEdgeModelPtr normal_y;
        WeakEdgeModelPtr normal_z;
};

#endif
