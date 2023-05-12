/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "MeshLoaderStructs.hh"

namespace dsMesh {
namespace {
#if 0
template <typename T> void DeletePointersFromVector(T &x)
{
    typename T::iterator it = x.begin();
    for ( ; it != x.end(); ++it)
    {
        delete *it;
    }
}
#endif

template <typename T> void DeletePointersFromMap(T &x)
{
    typename T::iterator it = x.begin();
    for ( ; it != x.end(); ++it)
    {
        delete it->second;
    }
}
}

const char * Solution::ModelTypeString[] = {
    "MUNDEFINED",
    "Node",
    "Edge",
    "Triangle Edge",
    "Tetrahedron Edge",
    "Interface Node"};

MeshContact::~MeshContact()
{
//    DeletePointersFromMap<MeshSolutionList_t>(solutionList);
    DeletePointersFromMap<MeshEquationList_t>(equationList);
}


MeshInterface::~MeshInterface()
{
    DeletePointersFromMap<MeshSolutionList_t>(solutionList);
    DeletePointersFromMap<MeshEquationList_t>(equationList);
}

MeshRegion::~MeshRegion()
{
    DeletePointersFromMap<MeshSolutionList_t>(solutionList);
    DeletePointersFromMap<MeshEquationList_t>(equationList);
}

void MeshRegion::DecomposeAndUniquify()
{
  // This builds lower order elements, but not nodes

  // element nodes are guaranteed to be sorted when constructed
  if (!tetrahedra.empty() && triangles.empty())
  {
    for (MeshTetrahedronList_t::iterator it = tetrahedra.begin(); it != tetrahedra.end(); ++it)
    {
      MeshTetrahedron &t = *it;
      triangles.push_back(MeshTriangle(t.Index0(), t.Index1(), t.Index2()));
      triangles.push_back(MeshTriangle(t.Index0(), t.Index1(), t.Index3()));
      triangles.push_back(MeshTriangle(t.Index0(), t.Index2(), t.Index3()));
      triangles.push_back(MeshTriangle(t.Index1(), t.Index2(), t.Index3()));
    }
    // sort and collapse
    std::sort(triangles.begin(), triangles.end());
    MeshTriangleList_t::iterator nonewend = std::unique(triangles.begin(), triangles.end());
    triangles.erase(nonewend, triangles.end());

  }
  if (!triangles.empty() && edges.empty())
  {
    for (MeshTriangleList_t::iterator it = triangles.begin(); it != triangles.end(); ++it)
    {
      MeshTriangle &t = *it;
      edges.push_back(MeshEdge(t.Index0(), t.Index1()));
      edges.push_back(MeshEdge(t.Index0(), t.Index2()));
      edges.push_back(MeshEdge(t.Index1(), t.Index2()));
    }
    std::sort(edges.begin(), edges.end());
    MeshEdgeList_t::iterator nonewend = std::unique(edges.begin(), edges.end());
    edges.erase(nonewend, edges.end());
  }

  if (!edges.empty() && nodes.empty())
  {
    for (MeshEdgeList_t::iterator it = edges.begin(); it != edges.end(); ++it)
    {
      MeshEdge e = *it;
      nodes.push_back(MeshNode(e.Index0()));
      nodes.push_back(MeshNode(e.Index1()));
    }
    std::sort(nodes.begin(), nodes.end());
    MeshNodeList_t::iterator nonewend = std::unique(nodes.begin(), nodes.end());
    nodes.erase(nonewend, nodes.end());
  }
}

void MeshContact::DecomposeAndUniquify()
{
  region.DecomposeAndUniquify();
}

void MeshInterface::DecomposeAndUniquify()
{
  region0.DecomposeAndUniquify();
  region1.DecomposeAndUniquify();
}

}

