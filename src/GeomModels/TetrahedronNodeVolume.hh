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
        void Serialize(std::ostream &) const;

    private:
        friend class dsModelFactory<TetrahedronNodeVolume>;
        explicit TetrahedronNodeVolume(RegionPtr);

        void derived_init();

        void   calcTetrahedronEdgeScalarValues() const;
        void   setInitialValues();
};

#endif

