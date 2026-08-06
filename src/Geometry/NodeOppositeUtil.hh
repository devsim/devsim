/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef NODEOPPOSITEUTIL_HH
#define NODEOPPOSITEUTIL_HH
class Node;
class Edge;
class Triangle;
class Tetrahedron;

namespace NodeOppositeUtil {
const Node *findNodeOppositeOfTriangleEdge(const Edge &edge, const Triangle &triangle);
const Node *findNodeOppositeOfTetrahedronTriangle(const Triangle &triangle, const Tetrahedron &tetrahedron);
}

#endif
