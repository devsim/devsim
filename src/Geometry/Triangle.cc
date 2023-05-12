/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "Triangle.hh"
#include "Node.hh"
#include "dsAssert.hh"

#include <cmath>
#include <array>

using std::abs;

const double Triangle::EPSILON=1.0e-20;
Triangle::Triangle(size_t ind, ConstNodePtr n0, ConstNodePtr n1, ConstNodePtr n2) : nodes(3)
{
    index = ind;
    nodes[0]=n0;
    nodes[1]=n1;
    nodes[2]=n2;

}

const std::vector<ConstNodePtr> &Triangle::GetFENodeList() const
{
  if (!fe_nodes.empty())
  {
    return fe_nodes;
  }

  // principle directions
  static const Vector<double> pdirs[] =
  {
    Vector<double>(0.0, 0.0, 1.0),
    Vector<double>(0.0, 1.0, 0.0),
    Vector<double>(1.0, 0.0, 0.0)
  };

  fe_nodes.resize(3);
  for (size_t i = 0; i < 3; ++i)
  {
    fe_nodes[i] = nodes[i];
  }

  std::array<Vector<double>, 3> positions;
  for (size_t i = 0; i < 3; ++i)
  {
    positions[i] = ConvertPosition<double>(nodes[i]->Position());
  }

  std::array<Vector<double>, 2> vecs;
  for (size_t i = 0; i < 2; ++i)
  {
    vecs[i] = positions[i+1] - positions[i];
  }
  const auto &cp = vecs[0].cross_prod(vecs[1]);

  double dir = 0.0;
  // is this sign correct for output normal?
  // only really works well in plane
  for (auto &v : pdirs)
  {
    dir = cp.dot_prod(v);
    if (dir != 0.0)
    {
      break;
    }
  }

  if (dir < 0.0)
  {
    std::swap(fe_nodes[1], fe_nodes[2]);
  }

  return fe_nodes;
}

template <typename DoubleType>
Vector<DoubleType> GetCenter(const Triangle &tr)
{
  const std::vector<ConstNodePtr> &nodes = tr.GetNodeList();

  const Vector<DoubleType> &v0 = ConvertPosition<DoubleType>(nodes[0]->Position());
  const Vector<DoubleType> &v1 = ConvertPosition<DoubleType>(nodes[1]->Position());
  const Vector<DoubleType> &v2 = ConvertPosition<DoubleType>(nodes[2]->Position());

  return GetTriangleCenter(v0, v1, v2);
}

//// Special case where we are not in a plane
template <typename DoubleType>
Vector<DoubleType> GetTriangleCenter3d(const Vector<DoubleType> &p1, const Vector<DoubleType> &p2, const Vector<DoubleType> &p3)
{

// TODO:"This formula is from wikipedia and has not been verified"

  Vector<DoubleType> p12 = p1 - p2;
  Vector<DoubleType> p13 = p1 - p3;
  Vector<DoubleType> p23 = p2 - p3;

  //// Watch out for colinearity here
  const Vector<DoubleType> tden1 = cross_prod(p12, p23);
  const DoubleType den = 2 * dot_prod(tden1, tden1);

  const DoubleType deninv = 1.0 / den;

  const DoubleType alpha = dot_prod(p23, p23) * dot_prod(p12, p13) * deninv;

  const DoubleType beta  = dot_prod(p13, p13) * dot_prod(-p12, p23) * deninv;

  /// dot_prod(p31, p32) = dot_prod(p13, p23)
  const DoubleType gamma = dot_prod(p12, p12) * dot_prod(p13, p23) * deninv;

  Vector<DoubleType> pc = alpha * p1 + beta * p2 + gamma * p3;

  return pc;
}

template <typename DoubleType>
Vector<DoubleType> GetTriangleCenterInPlane(DoubleType x0, DoubleType x1, DoubleType x2, DoubleType y0, DoubleType y1, DoubleType y2)
{
  DoubleType xcenter = 0.0;
  DoubleType ycenter = 0.0;

  if (abs(y1-y0) < Triangle::EPSILON)
  {
    const DoubleType m1 = - (x2-x1) / (y2-y1);
    const DoubleType mx1 = (x1 + x2) / 2.0;
    const DoubleType my1 = (y1 + y2) / 2.0;
    xcenter = (x1 + x0) / 2.0;
    ycenter = m1 * (xcenter - mx1) + my1;
  }
  else if (abs(y2-y1) < Triangle::EPSILON)
  {
    const DoubleType m0 = - (x1-x0) / (y1-y0);
    const DoubleType mx0 = (x0 + x1) / 2.0;
    const DoubleType my0 = (y0 + y1) / 2.0;
    xcenter = (x2 + x1) / 2.0;
    ycenter = m0 * (xcenter - mx0) + my0;
  }
  else
  {
    const DoubleType  m0 = - (x1-x0) / (y1-y0);
    const DoubleType  m1 = - (x2-x1) / (y2-y1);
    const DoubleType  mx0 = (x0 + x1) / 2.0;
    const DoubleType  mx1 = (x1 + x2) / 2.0;
    const DoubleType  my0 = (y0 + y1) / 2.0;
    const DoubleType  my1 = (y1 + y2) / 2.0;
    xcenter = (m0 * mx0 - m1 * mx1 + my1 - my0) / (m0 - m1);
    ycenter = m0 * (xcenter - mx0) + my0;
  }

  return Vector<DoubleType>(xcenter, ycenter);
}

//// Handles case where all points in a plane
template <typename DoubleType>
Vector<DoubleType> GetTriangleCenter(const Vector<DoubleType> &v0, const Vector<DoubleType> &v1, const Vector<DoubleType> &v2)
{
  const Vector<DoubleType> &v10 = v1 - v0;
  const Vector<DoubleType> &v20 = v2 - v0;

  Vector<DoubleType> ret;

  if ((abs(v10.Getz()) < Triangle::EPSILON) && (abs(v20.Getz()) < Triangle::EPSILON))
  {
    Vector<DoubleType> temp = GetTriangleCenterInPlane(v0.Getx(), v1.Getx(), v2.Getx(), v0.Gety(), v1.Gety(), v2.Gety());
    ret = Vector<DoubleType>(temp.Getx(), temp.Gety(), v0.Getz());
  }
  else if ((abs(v10.Gety()) < Triangle::EPSILON) && (abs(v20.Gety()) < Triangle::EPSILON))
  {
    Vector<DoubleType> temp = GetTriangleCenterInPlane(v0.Getx(), v1.Getx(), v2.Getx(), v0.Getz(), v1.Getz(), v2.Getz());
    ret = Vector<DoubleType>(temp.Getx(), v0.Gety(), temp.Gety());
  }
  else if ((abs(v10.Getx()) < Triangle::EPSILON) && (abs(v20.Getx()) < Triangle::EPSILON))
  {
    Vector<DoubleType> temp = GetTriangleCenterInPlane(v0.Getz(), v1.Getz(), v2.Getz(), v0.Gety(), v1.Gety(), v2.Gety());
    ret = Vector<DoubleType>(v0.Getx(), temp.Gety(), temp.Getx());
  }
  else
  {
    ret = GetTriangleCenter3d(v0, v1, v2);
  }
  return ret;
}

template Vector<double> GetCenter(const Triangle &);
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template Vector<float128> GetCenter(const Triangle &);
#endif

