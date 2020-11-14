/***
DEVSIM
Copyright 2013 Devsim LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
***/

#ifndef EDGE_HH
#define EDGE_HH

#include <cstddef>
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

struct EdgeCompIndex
{
   bool operator()(ConstEdgePtr x, ConstEdgePtr y) { return x->GetIndex() < y->GetIndex(); }
};

#endif
