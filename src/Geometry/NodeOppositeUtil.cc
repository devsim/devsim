#include "NodeOppositeUtil.hh"
#include "Node.hh"
#include "Edge.hh"
#include "Triangle.hh"

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
}

