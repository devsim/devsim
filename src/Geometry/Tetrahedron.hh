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
