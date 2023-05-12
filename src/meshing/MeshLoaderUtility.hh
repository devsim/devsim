/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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

