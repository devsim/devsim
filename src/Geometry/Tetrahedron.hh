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

#ifndef TETRAHEDRON_HH
#define TETRAHEDRON_HH

#include "Vector.hh"

#include <cstddef>
#include <functional>
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


   private:

      Tetrahedron();
      Tetrahedron (const Tetrahedron &);
      Tetrahedron &operator= (const Tetrahedron &);

      size_t index;
      std::vector<ConstNodePtr> nodes; 
};

struct TetrahedronCompIndex : public std::binary_function<ConstTetrahedronPtr, ConstTetrahedronPtr, bool>
{
   bool operator()(ConstTetrahedronPtr x, ConstTetrahedronPtr y) { return x->GetIndex() < y->GetIndex(); }
};

Vector GetCenter(const Tetrahedron &);
Vector GetCenter(const std::vector<ConstNodePtr> &nodes);

#endif
