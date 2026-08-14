/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef EDGE_HH
#define EDGE_HH

#include <cstddef>
#include <array>

class Node;
typedef Node *NodePtr;
typedef const Node *ConstNodePtr;

class Edge;
typedef Edge *EdgePtr;
typedef const Edge *ConstEdgePtr;

/**
   An Edge is a one dimensional element bridging two nodes.  Higher order
   elements (Triangle, Prism, etc . . .) can refer to edges.  Data structures
   will be made available to index data wrt an edge belonging to an element
*/
class Edge {
   public:
      Edge(size_t ind, ConstNodePtr, ConstNodePtr);
      size_t GetIndex() const
      {
         return index;
      }

      void SetIndex(size_t i)
      {
         index = i;
      }

      const std::array<ConstNodePtr, 2> &GetNodeList() const
      {
          return nodes;
      }

      const std::array<ConstNodePtr, 2> &GetFENodeList() const
      {
          return nodes;
      }

      ConstNodePtr GetHead() const
      {
          return nodes[0];
      }

      ConstNodePtr GetTail() const
      {
          return nodes[1];
      }

      double GetNodeSign(ConstNodePtr) const;


   private:

      Edge();
      Edge (const Edge &);
      Edge &operator= (const Edge &);

      size_t index;
      std::array<ConstNodePtr, 2> nodes{};
};

struct EdgeCompIndex
{
   bool operator()(ConstEdgePtr x, ConstEdgePtr y) { return x->GetIndex() < y->GetIndex(); }
};

#endif
