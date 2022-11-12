/***
DEVSIM
Copyright 2013 DEVSIM LLC

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

#ifndef MESH_LOADER_UTILITY_HH
#define MESH_LOADER_UTILITY_HH
#include "MeshLoaderStructs.hh"
class Coordinate;
class Node;
class Edge;
class Triangle;
class Tetrahedron;
#include <vector>
namespace dsMesh {

struct Shapes {
  enum class ElementType_t {UNKNOWN = -1, POINT=0, LINE, TRIANGLE, TETRAHEDRON};

  typedef std::vector<int> NodeIndexes_t;

  void AddShape(ElementType_t element_type, const NodeIndexes_t &node_indexes);
  void AddShape(ElementType_t element_type, const int * node_indexes);

  void AddShapes(Shapes &);

  void DecomposeAndUniquify();

  size_t GetDimension() const;

  size_t GetNumberOfTypes() const;

  MeshNodeList_t        Points;
  MeshEdgeList_t        Lines;
  MeshTriangleList_t    Triangles;
  MeshTetrahedronList_t Tetrahedra;
};
}
#endif

