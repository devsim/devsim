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

#ifndef EDGE_HH
#define EDGE_HH

#include <cstddef>
#include <functional>
#include <vector>

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

      const std::vector<ConstNodePtr> &GetNodeList() const
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
      std::vector<ConstNodePtr> nodes;
};

struct EdgeCompIndex : public std::binary_function<ConstEdgePtr, ConstEdgePtr, bool>
{
   bool operator()(ConstEdgePtr x, ConstEdgePtr y) { return x->GetIndex() < y->GetIndex(); }
};

#endif
