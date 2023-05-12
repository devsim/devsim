/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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

