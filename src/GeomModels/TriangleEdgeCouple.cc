/***
DEVSIM
Copyright 2013 Devsim LLC

This file is part of DEVSIM.

DEVSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, version 3.

DEVSIM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with DEVSIM.  If not, see <http://www.gnu.org/licenses/>.
***/

#include "TriangleEdgeCouple.hh"
#include "Region.hh"
#include "dsAssert.hh"
#include "Vector.hh"
#include "Triangle.hh"
#include "Edge.hh"
#include "Node.hh"

TriangleEdgeCouple::TriangleEdgeCouple(RegionPtr rp) :
TriangleEdgeModel("ElementEdgeCouple", rp, TriangleEdgeModel::SCALAR)
{
}


void TriangleEdgeCouple::calcTriangleEdgeScalarValues() const
{
  const size_t dimension=GetRegion().GetDimension();

  dsAssert(dimension == 2, "UNEXPECTED");

  const ConstTriangleList &el = GetRegion().GetTriangleList();
  std::vector<double> ev(3*el.size());

  for (size_t i = 0; i < el.size(); ++i)
  {
    const Vector &v = calcTriangleEdgeCouple(el[i]);
    const size_t indexi = 3*i;
    ev[indexi]     = v.Getx();
    ev[indexi + 1] = v.Gety();
    ev[indexi + 2] = v.Getz();
  }

  SetValues(ev);
}

Vector TriangleEdgeCouple::calcTriangleEdgeCouple(ConstTrianglePtr tp) const
{
    const Triangle &triangle = *tp;
    const std::vector<Vector> &centers = GetRegion().GetTriangleCenters();

    const Region::TriangleToConstEdgeList_t &ttelist = GetRegion().GetTriangleToEdgeList();
    size_t tindex = triangle.GetIndex();
    const ConstEdgeList &edgeList = ttelist[tindex];

    const Vector &vc = centers[triangle.GetIndex()];

    double ec[3];

    for (size_t i = 0; i < 3; ++i)
    {
      const Edge &edge = *edgeList[i];

      const Vector &p0 = edge.GetHead()->Position();
      const Vector &p1 = edge.GetTail()->Position();

      Vector vm = p0;
      vm += p1;
      vm *= 0.5;
      vm -= vc;
      ec[i] = vm.magnitude();
      // TODO: think about what happens if we are outside the triangle
    }

    return Vector(ec[0], ec[1], ec[2]);
}

void TriangleEdgeCouple::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

