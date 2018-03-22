
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
void processEdge(const Edge &edge, const EdgeScalarList<DoubleType> &unitx, const EdgeScalarList<DoubleType> &unity, const EdgeScalarList<DoubleType> &edgeLengths, std::vector<DoubleType> &nv, std::vector<DoubleType> &nvx, std::vector<DoubleType> &nvy);

template<typename DoubleType>
void processEdgeList(ConstEdgeList &edge_list, const EdgeScalarList<DoubleType> &unitx, const EdgeScalarList<DoubleType> &unity, const EdgeScalarList<DoubleType> &edgeLengths, std::vector<DoubleType> &nv, std::vector<DoubleType> &nvx, std::vector<DoubleType> &nvy);

template <typename DoubleType>
void processTriangle(const Triangle &triangle, const std::vector<Vector<DoubleType>> &triangleCenters, std::vector<DoubleType> &nv, std::vector<DoubleType> &nvx, std::vector<DoubleType> &nvy, std::vector<DoubleType> &nvz);

template <typename DoubleType>
void processTriangleList(ConstTriangleList &triangle_list, const std::vector<Vector<DoubleType>> &triangleCenters, std::vector<DoubleType> &nv, std::vector<DoubleType> &nvx, std::vector<DoubleType> &nvy, std::vector<DoubleType> &nvz);
}
#endif

