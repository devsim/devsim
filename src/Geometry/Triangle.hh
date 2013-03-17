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

#ifndef TRIANGLE_HH
#define TRIANGLE_HH

#include "Vector.hh"

#include <cstddef>
#include <functional>
#include <vector>

class Node;
typedef Node *NodePtr;
typedef const Node *ConstNodePtr;

class Triangle;
typedef Triangle *TrianglePtr;
typedef const Triangle *ConstTrianglePtr;

typedef std::vector<TrianglePtr> TriangleList;

class Triangle {
   public:
      static const double EPSILON;

      Triangle(size_t ind, ConstNodePtr, ConstNodePtr, ConstNodePtr);

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

      Triangle();
      Triangle (const Triangle &);
      Triangle &operator= (const Triangle &);

      size_t index;
      std::vector<ConstNodePtr> nodes; 
};

struct TriangleCompIndex : public std::binary_function<ConstTrianglePtr, ConstTrianglePtr, bool>
{
   bool operator()(ConstTrianglePtr x, ConstTrianglePtr y) { return x->GetIndex() < y->GetIndex(); }
};

Vector GetCenter(const Triangle &);
Vector GetTriangleCenter(const Vector &, const Vector &, const Vector &);
Vector GetTriangleCenter2d(const Vector &, const Vector &, const Vector &);
Vector GetTriangleCenter3d(const Vector &, const Vector &, const Vector &);
#endif
