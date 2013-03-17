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

#ifndef NODE_HH
#define NODE_HH

#include "Coordinate.hh"
#include <cstddef>
#include <functional>

class Node;
typedef Node *NodePtr;
typedef const Node *ConstNodePtr;

/**
   A node belongs to a region of some kind.  It has an index which
   determines it's order.  Need to determine whether its place in
   Region::nodeList_ has to correspond to its node index, or if we
   are allowed to renumber nodes in special situations
*/
typedef const Coordinate *ConstCoordinatePtr;
class Node {
   public:

      // does a node really need to know if it is at an interface
      // add accessors later
      Node(size_t, ConstCoordinatePtr);

      size_t GetIndex() const
      {
         return index;
      }
      
      void SetIndex(size_t i)
      {
         index = i;
      }

      const Vector &Position() const
      {
         return coord->Position();
      }

      const Coordinate& GetCoordinate() const
      {
        return *coord;
      }

   private:

      Node();
      Node (const Node &);
      Node &operator= (const Node &);

      size_t             index;
      ConstCoordinatePtr coord;
};

struct NodeCompIndex : public std::binary_function<ConstNodePtr, ConstNodePtr, bool>
{
   bool operator()(ConstNodePtr x, ConstNodePtr y) { return x->GetIndex() < y->GetIndex(); }
};

#endif
