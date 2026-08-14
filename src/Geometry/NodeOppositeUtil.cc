#include "NodeOppositeUtil.hh"
#include "Node.hh"
#include "Edge.hh"
#include "Triangle.hh"
#include "Tetrahedron.hh"

namespace NodeOppositeUtil {
const Node *findNodeOppositeOfTriangleEdge(const Edge &edge, const Triangle &triangle)
{
  const Node * const h = edge.GetHead();
  const Node * const t = edge.GetTail();

  const Node *ret = nullptr;

  const auto &tnl = triangle.GetNodeList();
  for (size_t i = 0; i < 3; ++i)
  {
    const Node *tnode = tnl[i];
    if ((tnode != h) && (tnode != t))
    {
      ret = tnode;
      break;
    }
  }
  return ret;
}

const Node *findNodeOppositeOfTetrahedronTriangle(const Triangle &triangle, const Tetrahedron &tetrahedron)
{
  const auto &tnl = triangle.GetNodeList();
  const auto &tetnl = tetrahedron.GetNodeList();

  const Node *ret = nullptr;

  for (size_t i = 0; i < 4; ++i)
  {
    const Node *tnode = tetnl[i];
    if ((tnode != tnl[0]) && (tnode != tnl[1]) && (tnode != tnl[2]))
    {
      ret = tnode;
      break;
    }
  }
  return ret;
}

}