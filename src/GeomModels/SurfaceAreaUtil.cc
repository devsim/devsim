#include "SurfaceAreaUtil.hh"
#include "Node.hh"
#include "Edge.hh"
#include "Triangle.hh"

#include <vector>

namespace SurfaceAreaUtil
{
template<typename DoubleType>
void processEdge(const Edge &edge, const EdgeScalarList<DoubleType> &unitx, const EdgeScalarList<DoubleType> &unity, const EdgeScalarList<DoubleType> &edgeLengths, std::vector<DoubleType> &nv, std::vector<DoubleType> &nvx, std::vector<DoubleType> &nvy)
{
  const size_t ei = edge.GetIndex();

  const Node &node0 = *edge.GetHead();
  const Node &node1 = *edge.GetTail();

  const size_t ni0  = node0.GetIndex();
  const size_t ni1  = node1.GetIndex();

  const DoubleType length = 0.5*edgeLengths[ei];
  nv[ni0] += length;
  nv[ni1] += length;


  Vector<DoubleType> vvec(unity[ei]*length, -unitx[ei]*length, 0.0);

  Vector<DoubleType> un0(nvx[ni0], nvy[ni0]);
  if (dot_prod(vvec, un0) < 0.0)
  {
    nvx[ni0] -= vvec.Getx();
    nvy[ni0] -= vvec.Gety();
  }
  else
  {
    nvx[ni0] += vvec.Getx();
    nvy[ni0] += vvec.Gety();
  }

  Vector<DoubleType> un1(nvx[ni1], nvy[ni1]);
  if (dot_prod(vvec, un1) < 0.0)
  {
    nvx[ni1] -= vvec.Getx();
    nvy[ni1] -= vvec.Gety();
  }
  else
  {
    nvx[ni1] += vvec.Getx();
    nvy[ni1] += vvec.Gety();
  }
}

template<typename DoubleType>
void processEdgeList(ConstEdgeList &edge_list, const EdgeScalarList<DoubleType> &unitx, const EdgeScalarList<DoubleType> &unity, const EdgeScalarList<DoubleType> &edgeLengths, std::vector<DoubleType> &nv, std::vector<DoubleType> &nvx, std::vector<DoubleType> &nvy)
{
  std::vector<size_t> edge_visited(unitx.size());
  for (ConstEdgeList::const_iterator it = edge_list.begin(); it != edge_list.end(); ++it)
  {
    const Edge &edge = *(*it);

    //// Here we ignore edges we already visited
    const size_t ei = edge.GetIndex();
    if (edge_visited[ei] == 0)
    {
      edge_visited[ei] = 1;
    }
    else
    {
      continue;
    }

    processEdge(edge, unitx, unity, edgeLengths, nv, nvx, nvy);

  }
  for (size_t i = 0; i < nv.size(); ++i)
  {
    const DoubleType area = nv[i];
    if (area > 0.0)
    {
      const DoubleType wt = magnitude(Vector<DoubleType>(nvx[i], nvy[i], 0.0));
      if (wt > 0.0)
      {
        nvx[i] /= wt;
        nvy[i] /= wt;
      }
    }
  }
}

template <typename DoubleType>
void ProcessAreaAndNormal(size_t ni0, std::vector<DoubleType> &nv, std::vector<DoubleType> &nvx, std::vector<DoubleType> &nvy, std::vector<DoubleType> &nvz, const Vector<DoubleType> &vnormal, DoubleType volume)
{
  nv[ni0] += volume;

  Vector<DoubleType> uvec(nvx[ni0], nvy[ni0], nvz[ni0]);

  if (dot_prod(uvec, vnormal) < 0.0)
  {
    nvx[ni0] -= vnormal.Getx();
    nvy[ni0] -= vnormal.Gety();
    nvz[ni0] -= vnormal.Getz();
  }
  else
  {
    nvx[ni0] += vnormal.Getx();
    nvy[ni0] += vnormal.Gety();
    nvz[ni0] += vnormal.Getz();
  }
}

template <typename DoubleType>
void processTriangle(const Triangle &triangle, const std::vector<Vector<DoubleType>> &triangleCenters, std::vector<DoubleType> &nv, std::vector<DoubleType> &nvx, std::vector<DoubleType> &nvy, std::vector<DoubleType> &nvz)
{
    const ConstNodeList &nodeList = triangle.GetNodeList();

    const size_t &ni0 = nodeList[0]->GetIndex();
    const size_t &ni1 = nodeList[1]->GetIndex();
    const size_t &ni2 = nodeList[2]->GetIndex();

    const size_t ti = triangle.GetIndex();
    const Vector<DoubleType> &triangleCenter = triangleCenters[ti];

    const auto &h0 = nodeList[0]->Position();
    const auto &h1 = nodeList[1]->Position();
    const auto &h2 = nodeList[2]->Position();

    const Vector<DoubleType> np0(h0.Getx(), h0.Gety(), h0.Getz());
    const Vector<DoubleType> np1(h1.Getx(), h1.Gety(), h1.Getz());
    const Vector<DoubleType> np2(h2.Getx(), h2.Gety(), h2.Getz());

    //// vector from triangle center to edge node
    //// vector between edge nodes
    //// area is 0.5 * base * height
    //// area split between both nodes
    const static DoubleType quarter = 0.25;

    const Vector<DoubleType> &v01   = np0 - np1;
    const Vector<DoubleType> &vc01  = np0 - triangleCenter;
    const Vector<DoubleType> &vec01 = quarter * cross_prod(v01, vc01);
    const DoubleType  vol01 = magnitude(vec01);

    ProcessAreaAndNormal(ni0, nv, nvx, nvy, nvz, vec01, vol01);
    ProcessAreaAndNormal(ni1, nv, nvx, nvy, nvz, vec01, vol01);

    const Vector<DoubleType> &v02   = np0 - np2;
    const Vector<DoubleType> &vc02  = np0 - triangleCenter;
    const Vector<DoubleType> &vec02 = quarter * cross_prod(v02, vc02);
    const DoubleType  vol02 = magnitude(vec02);

    ProcessAreaAndNormal(ni0, nv, nvx, nvy, nvz, vec02, vol02);
    ProcessAreaAndNormal(ni2, nv, nvx, nvy, nvz, vec02, vol02);

    const Vector<DoubleType> &v12   = np1 - np2;
    const Vector<DoubleType> &vc12  = np1 - triangleCenter;
    const Vector<DoubleType> &vec12 = quarter * cross_prod(v12, vc12);
    const DoubleType  vol12 = magnitude(vec12);

    ProcessAreaAndNormal(ni1, nv, nvx, nvy, nvz, vec12, vol12);
    ProcessAreaAndNormal(ni2, nv, nvx, nvy, nvz, vec12, vol12);
}

template <typename DoubleType>
void processTriangleList(ConstTriangleList &triangle_list, const std::vector<Vector<DoubleType>> &triangleCenters, std::vector<DoubleType> &nv, std::vector<DoubleType> &nvx, std::vector<DoubleType> &nvy, std::vector<DoubleType> &nvz)
{
  std::vector<size_t> triangle_visited(triangleCenters.size());
  for (ConstTriangleList::const_iterator it = triangle_list.begin(); it != triangle_list.end(); ++it)
  {
    const Triangle &triangle = *(*it);

    //// Here we ignore triangles we already visited
    const size_t ti = triangle.GetIndex();
    if (triangle_visited[ti] == 0)
    {
      triangle_visited[ti] = 1;
    }
    else
    {
      continue;
    }

    processTriangle(triangle, triangleCenters, nv, nvx, nvy, nvz);
  }

  for (size_t i = 0; i < nv.size(); ++i)
  {
    const DoubleType area = nv[i];
    if (area > 0.0)
    {
      const DoubleType wt = magnitude(Vector<DoubleType>(nvx[i], nvy[i], nvz[i]));
      if (wt > 0.0)
      {
        nvx[i] /= wt;
        nvy[i] /= wt;
        nvz[i] /= wt;
      }
    }
  }
}

}

namespace SurfaceAreaUtil {
template
void processEdgeList<>(ConstEdgeList &edge_list, const EdgeScalarList<double> &unitx, const EdgeScalarList<double> &unity, const EdgeScalarList<double> &edgeLengths, std::vector<double> &nv, std::vector<double> &nvx, std::vector<double> &nvy);

template
void processTriangleList<>(ConstTriangleList &triangle_list, const std::vector<Vector<double>> &triangleCenters, std::vector<double> &nv, std::vector<double> &nvx, std::vector<double> &nvy, std::vector<double> &nvz);
}
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
namespace SurfaceAreaUtil {
template
void processEdgeList<>(ConstEdgeList &edge_list, const EdgeScalarList<float128> &unitx, const EdgeScalarList<float128> &unity, const EdgeScalarList<float128> &edgeLengths, std::vector<float128> &nv, std::vector<float128> &nvx, std::vector<float128> &nvy);

template
void processTriangleList<>(ConstTriangleList &triangle_list, const std::vector<Vector<float128>> &triangleCenters, std::vector<float128> &nv, std::vector<float128> &nvx, std::vector<float128> &nvy, std::vector<float128> &nvz);
}
#endif

