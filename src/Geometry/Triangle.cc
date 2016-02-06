/***
DEVSIM
Copyright 2013 Devsim LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
***/

#include "Triangle.hh"
#include "Node.hh"
#include "dsAssert.hh"
//#include "DenseMatrix.hh"
#include <cmath>
const double Triangle::EPSILON=1.0e-20;
Triangle::Triangle(size_t ind, ConstNodePtr n0, ConstNodePtr n1, ConstNodePtr n2) : nodes(3)
{
    index = ind;
    nodes[0]=n0;
    nodes[1]=n1;
    nodes[2]=n2;

}

Vector GetCenter(const Triangle &tr)
{
  const std::vector<ConstNodePtr> &nodes = tr.GetNodeList();

  const Vector &v0 = nodes[0]->Position();
  const Vector &v1 = nodes[1]->Position();
  const Vector &v2 = nodes[2]->Position();

  return GetTriangleCenter(v0, v1, v2);
}

//// Special case where we are not in a plane
Vector GetTriangleCenter3d(const Vector &p1, const Vector &p2, const Vector &p3)
{

// TODO:"This formula is from wikipedia and has not been verified"

  Vector p12 = p1 - p2;
  Vector p13 = p1 - p3;
  Vector p23 = p2 - p3;

  //// Watch out for colinearity here
  const Vector tden1 = cross_prod(p12, p23);
  const double den = 2 * dot_prod(tden1, tden1);

  const double deninv = 1.0 / den;

  const double alpha = dot_prod(p23, p23) * dot_prod(p12, p13) * deninv;

  const double beta  = dot_prod(p13, p13) * dot_prod(-p12, p23) * deninv;

  /// dot_prod(p31, p32) = dot_prod(p13, p23)
  const double gamma = dot_prod(p12, p12) * dot_prod(p13, p23) * deninv;

  Vector pc = alpha * p1 + beta * p2 + gamma * p3;

  return pc; 
}

Vector GetTriangleCenterInPlane(double x0, double x1, double x2, double y0, double y1, double y2)
{
  double xcenter = 0.0;
  double ycenter = 0.0;

  if (fabs(y1-y0) < Triangle::EPSILON)
  {
    const double m1 = - (x2-x1) / (y2-y1);
    const double mx1 = (x1 + x2) / 2.0;
    const double my1 = (y1 + y2) / 2.0;
    xcenter = (x1 + x0) / 2.0;
    ycenter = m1 * (xcenter - mx1) + my1;
  }
  else if (fabs(y2-y1) < Triangle::EPSILON)
  {
    const double m0 = - (x1-x0) / (y1-y0);
    const double mx0 = (x0 + x1) / 2.0;
    const double my0 = (y0 + y1) / 2.0;
    xcenter = (x2 + x1) / 2.0;
    ycenter = m0 * (xcenter - mx0) + my0;
  }
  else
  {
    const double  m0 = - (x1-x0) / (y1-y0);
    const double  m1 = - (x2-x1) / (y2-y1);
    const double  mx0 = (x0 + x1) / 2.0;
    const double  mx1 = (x1 + x2) / 2.0;
    const double  my0 = (y0 + y1) / 2.0;
    const double  my1 = (y1 + y2) / 2.0;
    xcenter = (m0 * mx0 - m1 * mx1 + my1 - my0) / (m0 - m1);
    ycenter = m0 * (xcenter - mx0) + my0;
  }

  return Vector(xcenter, ycenter);
}

//// Handles case where all points in a plane
Vector GetTriangleCenter(const Vector &v0, const Vector &v1, const Vector &v2)
{
  const Vector &v10 = v1 - v0;
  const Vector &v20 = v2 - v0;

  Vector ret;

  if ((fabs(v10.Getz()) < Triangle::EPSILON) && (fabs(v20.Getz()) < Triangle::EPSILON))
  {
    Vector temp = GetTriangleCenterInPlane(v0.Getx(), v1.Getx(), v2.Getx(), v0.Gety(), v1.Gety(), v2.Gety());
    ret = Vector(temp.Getx(), temp.Gety(), v0.Getz());
  }
  else if ((fabs(v10.Gety()) < Triangle::EPSILON) && (fabs(v20.Gety()) < Triangle::EPSILON))
  {
    Vector temp = GetTriangleCenterInPlane(v0.Getx(), v1.Getx(), v2.Getx(), v0.Getz(), v1.Getz(), v2.Getz());
    ret = Vector(temp.Getx(), v0.Gety(), temp.Gety());
  }
  else if ((fabs(v10.Getx()) < Triangle::EPSILON) && (fabs(v20.Getx()) < Triangle::EPSILON))
  {
    Vector temp = GetTriangleCenterInPlane(v0.Getz(), v1.Getz(), v2.Getz(), v0.Gety(), v1.Gety(), v2.Gety());
    ret = Vector(v0.Getx(), temp.Gety(), temp.Getx());
  }
  else
  {
    ret = GetTriangleCenter3d(v0, v1, v2);
  }
  return ret;
}


