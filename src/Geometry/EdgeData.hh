/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef EDGEDATA_HH
#define EDGEDATA_HH
//// Used for knowing the two Triangles on each Tetrahedron edge
//// as well as the nodes opposite of those triangles
class EdgeData {
  public:
  EdgeData() : edge(nullptr)
  {
    triangle[0] = nullptr;
    triangle[1] = nullptr;
    /// The index is local to TetrahedronToTriangleList
    triangle_index[0] = size_t(-1);
    triangle_index[1] = size_t(-1);
    nodeopp[0] = nullptr;
    nodeopp[1] = nullptr;
  }
  ConstEdgePtr     edge;
  ConstTrianglePtr triangle[2];
  size_t           triangle_index[2];
  //// These are the nodes opposite of the edge on the triangle
  ConstNodePtr     nodeopp[2];
};
#endif
