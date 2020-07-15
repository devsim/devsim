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

      const Vector<double> &Position() const
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

template <typename DoubleType>
inline Vector<DoubleType> ConvertPosition(const Vector<double> &v)
{
  return Vector<DoubleType>(
    static_cast<DoubleType>(v.Getx()),
    static_cast<DoubleType>(v.Gety()),
    static_cast<DoubleType>(v.Getz()));
}

template <>
inline Vector<double> ConvertPosition(const Vector<double> &v)
{
  return v;
}

#endif

