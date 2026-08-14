/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef TRIANGLE_HH
#define TRIANGLE_HH

#include "Vector.hh"

#include <cstddef>
#include <vector>
#include <array>

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

      const std::array<ConstNodePtr, 3> &GetNodeList() const
      {
        return nodes;
      }

      std::array<ConstNodePtr, 3> GetFENodeList() const;


   private:

      Triangle();
      Triangle (const Triangle &);
      Triangle &operator= (const Triangle &);

      size_t index;
      std::array<ConstNodePtr, 3> nodes;
      mutable int fe_sign = 0;
};

struct TriangleCompIndex
{
   bool operator()(ConstTrianglePtr x, ConstTrianglePtr y) { return x->GetIndex() < y->GetIndex(); }
};

template <typename DoubleType>
Vector<DoubleType> GetCenter(const Triangle &);

template <typename DoubleType>
Vector<DoubleType> GetTriangleCenter(const Vector<DoubleType> &, const Vector<DoubleType> &, const Vector<DoubleType> &);

template <typename DoubleType>
Vector<DoubleType> GetTriangleCenter2d(const Vector<DoubleType> &, const Vector<DoubleType> &, const Vector<DoubleType> &);

template <typename DoubleType>
Vector<DoubleType> GetTriangleCenter3d(const Vector<DoubleType> &, const Vector<DoubleType> &, const Vector<DoubleType> &);
#endif

