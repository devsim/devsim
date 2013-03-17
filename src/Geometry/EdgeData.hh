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

#ifndef EDGEDATA_HH
#define EDGEDATA_HH
//// Used for knowing the two Triangles on each Tetrahedron edge
//// as well as the nodes opposite of those triangles
class EdgeData {
  public:
  EdgeData() : edge(NULL)
  {
    triangle[0] = NULL;
    triangle[1] = NULL;
    /// The index is local to TetrahedronToTriangleList
    triangle_index[0] = size_t(-1);
    triangle_index[1] = size_t(-1);
    nodeopp[0] = NULL;
    nodeopp[1] = NULL;
  }
  ConstEdgePtr     edge;
  ConstTrianglePtr triangle[2];
  size_t           triangle_index[2];
  //// These are the nodes opposite of the edge on the triangle
  ConstNodePtr     nodeopp[2];
};
#endif
