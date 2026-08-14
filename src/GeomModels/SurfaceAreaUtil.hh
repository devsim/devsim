
#ifndef SURFACE_AREA_UTIL_HH
#define SURFACE_AREA_UTIL_HH
#include <vector>

class Node;

typedef std::vector<const Node *> ConstNodeList;

class Edge;

typedef std::vector<const Edge *> ConstEdgeList;

class Triangle;

typedef std::vector<const Triangle *> ConstTriangleList;

template <typename T> class Vector;

template <typename T>
using EdgeScalarList = std::vector<T>;


namespace SurfaceAreaUtil
{
template<typename DoubleType>
void processEdge(const Edge &edge, const Node &node_opposite, const EdgeScalarList<DoubleType> &unitx, const EdgeScalarList<DoubleType> &unity, const EdgeScalarList<DoubleType> &edgeLengths, std::vector<DoubleType> &nv, std::vector<DoubleType> &nvx, std::vector<DoubleType> &nvy);

template<typename DoubleType>
void processEdgeList(const ConstEdgeList &edge_list, const ConstNodeList &node_opposite_list, const EdgeScalarList<DoubleType> &unitx, const EdgeScalarList<DoubleType> &unity, const EdgeScalarList<DoubleType> &edgeLengths, std::vector<DoubleType> &nv, std::vector<DoubleType> &nvx, std::vector<DoubleType> &nvy);

template <typename DoubleType>
void processTriangle(const Triangle &triangle, const Node &node_opposite, const std::vector<Vector<DoubleType>> &triangleCenters, std::vector<DoubleType> &nv, std::vector<DoubleType> &nvx, std::vector<DoubleType> &nvy, std::vector<DoubleType> &nvz);

template <typename DoubleType>
void processTriangleList(const ConstTriangleList &triangle_list, const ConstNodeList &node_opposite_list, const std::vector<Vector<DoubleType>> &triangleCenters, std::vector<DoubleType> &nv, std::vector<DoubleType> &nvx, std::vector<DoubleType> &nvy, std::vector<DoubleType> &nvz);
}
#endif

