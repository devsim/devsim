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

namespace NodeOppositeUtil {
const Node *findNodeOppositeOfTriangleEdge(const Edge &edge, const Triangle &triangle);
}

#endif
