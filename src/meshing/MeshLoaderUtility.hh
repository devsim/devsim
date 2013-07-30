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
  enum ElementType_t {UNKNOWN = -1, POINT=0, LINE, TRIANGLE, TETRAHEDRON};

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

