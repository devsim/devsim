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
