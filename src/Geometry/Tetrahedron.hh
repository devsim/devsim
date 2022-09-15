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
  Tetrahedron(size_t ind, ConstNodePtr, ConstNodePtr, ConstNodePtr,
              ConstNodePtr);

  size_t GetIndex() const { return index; }

  void SetIndex(size_t i) { index = i; }

  const std::vector<ConstNodePtr> &GetNodeList() const { return nodes; }

  const std::vector<ConstNodePtr> &GetFENodeList() const;

private:
  Tetrahedron();
  Tetrahedron(const Tetrahedron &);
  Tetrahedron &operator=(const Tetrahedron &);

  std::vector<ConstNodePtr> nodes;
  mutable std::vector<ConstNodePtr> fe_nodes;
  size_t index;
};

struct TetrahedronCompIndex {
  bool operator()(ConstTetrahedronPtr x, ConstTetrahedronPtr y) {
    return x->GetIndex() < y->GetIndex();
  }
};

template <typename DoubleType>
Vector<DoubleType> GetCenter(const Tetrahedron &);

template <typename DoubleType>
Vector<DoubleType> GetCenter(const std::vector<ConstNodePtr> &nodes);

#endif
