/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "MeshLoaderUtility.hh"
#include "dsAssert.hh"
#include "Node.hh"
#include "Edge.hh"
#include "Triangle.hh"
#include "Tetrahedron.hh"

namespace dsMesh {

void Shapes::DecomposeAndUniquify()
{
  std::sort(Tetrahedra.begin(), Tetrahedra.end());
  MeshTetrahedronList_t::iterator tetnewend = std::unique(Tetrahedra.begin(), Tetrahedra.end());
  Tetrahedra.erase(tetnewend, Tetrahedra.end());

  for (MeshTetrahedronList_t::iterator it = Tetrahedra.begin(); it != Tetrahedra.end(); ++it)
  {
    const MeshTetrahedron &tet = *it;
    size_t i0 = tet.Index0();
    size_t i1 = tet.Index1();
    size_t i2 = tet.Index2();
    size_t i3 = tet.Index3();

    Triangles.push_back(MeshTriangle(i0, i1, i2));
    Triangles.push_back(MeshTriangle(i0, i1, i3));
    Triangles.push_back(MeshTriangle(i0, i2, i3));
    Triangles.push_back(MeshTriangle(i1, i2, i3));
  }

  std::sort(Triangles.begin(), Triangles.end());
  MeshTriangleList_t::iterator trinewend = std::unique(Triangles.begin(), Triangles.end());
  Triangles.erase(trinewend, Triangles.end());

  for (MeshTriangleList_t::iterator it = Triangles.begin(); it != Triangles.end(); ++it)
  {
    const MeshTriangle &tri = *it;
    size_t i0 = tri.Index0();
    size_t i1 = tri.Index1();
    size_t i2 = tri.Index2();

    Lines.push_back(MeshEdge(i0, i1));
    Lines.push_back(MeshEdge(i0, i2));
    Lines.push_back(MeshEdge(i1, i2));
  }

  std::sort(Lines.begin(), Lines.end());
  MeshEdgeList_t::iterator linewend = std::unique(Lines.begin(), Lines.end());
  Lines.erase(linewend, Lines.end());

  for (MeshEdgeList_t::iterator it = Lines.begin(); it != Lines.end(); ++it)
  {
    const MeshEdge &edge = *it;
    size_t i0 = edge.Index0();
    size_t i1 = edge.Index1();

    Points.push_back(MeshNode(i0));
    Points.push_back(MeshNode(i1));
  }

  std::sort(Points.begin(), Points.end());
  MeshNodeList_t::iterator nonewend = std::unique(Points.begin(), Points.end());
  Points.erase(nonewend, Points.end());
}

void Shapes::AddShape(ElementType_t element_type, const int * node_indexes)
{
  if (element_type == ElementType_t::POINT)
  {
    Points.push_back(MeshNode(node_indexes[0]));
  }
  else if (element_type == ElementType_t::LINE)
  {
    Lines.push_back(MeshEdge(node_indexes[0], node_indexes[1]));
  }
  else if (element_type == ElementType_t::TRIANGLE)
  {
    Triangles.push_back(MeshTriangle(node_indexes[0], node_indexes[1], node_indexes[2]));
  }
  else if (element_type == ElementType_t::TETRAHEDRON)
  {
    Tetrahedra.push_back(MeshTetrahedron(node_indexes[0], node_indexes[1], node_indexes[2], node_indexes[3]));
  }
}

void Shapes::AddShape(ElementType_t element_type, const NodeIndexes_t &node_indexes)
{
  AddShape(element_type, &node_indexes[0]);
}

size_t Shapes::GetDimension() const
{
  size_t dimension = 0;

  if (!Tetrahedra.empty())
  {
    dimension = 3;
  }
  else if (!Triangles.empty())
  {
    dimension = 2;
  }
  else if (!Lines.empty())
  {
    dimension = 1;
  }
  else if (!Points.empty())
  {
    dimension = 0;
  }

  return dimension;
}

size_t Shapes::GetNumberOfTypes() const
{
  size_t num_types = 0;

  if (!Tetrahedra.empty())
  {
    ++num_types;
  }

  if (!Triangles.empty())
  {
    ++num_types;
  }

  if (!Lines.empty())
  {
    ++num_types;
  }

  if (!Points.empty())
  {
    ++num_types;
  }

  return num_types;
}

void Shapes::AddShapes(Shapes &other_shapes)
{
  if (Points.empty())
  {
    Points.swap(other_shapes.Points);
  }
  else
  {
    for (MeshNodeList_t::iterator it = other_shapes.Points.begin(); it != other_shapes.Points.end(); ++it)
    {
      Points.push_back(*it);
    }

    MeshNodeList_t t;
    t.swap(other_shapes.Points);
  }

  if (Lines.empty())
  {
    Lines.swap(other_shapes.Lines);
  }
  else
  {
    for (MeshEdgeList_t::iterator it = other_shapes.Lines.begin(); it != other_shapes.Lines.end(); ++it)
    {
      Lines.push_back(*it);
    }

    MeshEdgeList_t t;
    t.swap(other_shapes.Lines);
  }

  if (Triangles.empty())
  {
    Triangles.swap(other_shapes.Triangles);
  }
  else
  {
    for (MeshTriangleList_t::iterator it = other_shapes.Triangles.begin(); it != other_shapes.Triangles.end(); ++it)
    {
      Triangles.push_back(*it);
    }

    MeshTriangleList_t t;
    t.swap(other_shapes.Triangles);
  }

  if (Tetrahedra.empty())
  {
    Tetrahedra.swap(other_shapes.Tetrahedra);
  }
  else
  {
    for (MeshTetrahedronList_t::iterator it = other_shapes.Tetrahedra.begin(); it != other_shapes.Tetrahedra.end(); ++it)
    {
      Tetrahedra.push_back(*it);
    }

    MeshTetrahedronList_t t;
    t.swap(other_shapes.Tetrahedra);
  }
}
}
