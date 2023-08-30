/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef TETRAHEDRON_HH
#define TETRAHEDRON_HH

#include "Vector.hh"

#include <cstddef>
#include <vector>

class Node;
typedef Node *NodePtr;
typedef const Node *ConstNodePtr;

class Tetrahedron;
typedef Tetrahedron *TetrahedronPtr;
typedef const Tetrahedron *ConstTetrahedronPtr;

typedef std::vector<TetrahedronPtr> TetrahedronList;

class Tetrahedron {
   public:
      Tetrahedron(size_t ind, ConstNodePtr, ConstNodePtr, ConstNodePtr, ConstNodePtr);

      size_t GetIndex() const
      {
         return index;
      }

      void SetIndex(size_t i)
      {
         index = i;
      }

      const std::vector<ConstNodePtr> &GetNodeList() const
      {
        return nodes;
      }

      const std::vector<ConstNodePtr> &GetFENodeList() const;

   private:

      Tetrahedron();
      Tetrahedron (const Tetrahedron &);
      Tetrahedron &operator= (const Tetrahedron &);

      std::vector<ConstNodePtr> nodes;
      mutable std::vector<ConstNodePtr> fe_nodes;
      size_t index;
};

struct TetrahedronCompIndex
{
   bool operator()(ConstTetrahedronPtr x, ConstTetrahedronPtr y) { return x->GetIndex() < y->GetIndex(); }
};

template <typename DoubleType>
Vector<DoubleType> GetCenter(const Tetrahedron &);

template <typename DoubleType>
Vector<DoubleType> GetTetrahedronCenter(const std::vector<ConstNodePtr> &nodes);

#endif

