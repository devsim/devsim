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
#include <array>
#include <span>

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

      const std::array<ConstNodePtr, 4> &GetNodeList() const
      {
        return nodes;
      }

      std::array<ConstNodePtr, 4> GetFENodeList() const;

   private:

      Tetrahedron();
      Tetrahedron (const Tetrahedron &);
      Tetrahedron &operator= (const Tetrahedron &);

      std::array<ConstNodePtr, 4> nodes;
      size_t index;
      mutable int fe_sign = 0;
};

struct TetrahedronCompIndex
{
   bool operator()(ConstTetrahedronPtr x, ConstTetrahedronPtr y) { return x->GetIndex() < y->GetIndex(); }
};

template <typename DoubleType>
Vector<DoubleType> GetCenter(const Tetrahedron &);

template <typename DoubleType>
Vector<DoubleType> GetTetrahedronCenter(std::span<ConstNodePtr> nodes);

#endif

