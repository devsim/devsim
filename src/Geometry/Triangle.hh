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

      const std::vector<ConstNodePtr> &GetFENodeList() const;


   private:

      Triangle();
      Triangle (const Triangle &);
      Triangle &operator= (const Triangle &);

      size_t index;
      std::vector<ConstNodePtr> nodes;
      mutable std::vector<ConstNodePtr> fe_nodes;
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

