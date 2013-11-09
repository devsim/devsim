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

#include "EdgeCouple.hh"
#include "Region.hh"
#include "Edge.hh"
#include "dsAssert.hh"
#include "TriangleEdgeModel.hh"
#include "TetrahedronEdgeModel.hh"

EdgeCouple::EdgeCouple(RegionPtr rp) :
EdgeModel("EdgeCouple", rp, EdgeModel::SCALAR)
{
  const size_t dimension = rp->GetDimension();
  if ((dimension == 2) || (dimension == 3))
  {
    RegisterCallback("ElementEdgeCouple");
  }
}


void EdgeCouple::calcEdgeScalarValues() const
{
  const size_t dimension=GetRegion().GetDimension();

  if (dimension == 1)
  {
    std::vector<double> ev(GetRegion().GetNumberEdges(), 1.0);
    SetValues(ev);
  }
  else if (dimension == 2)
  {
    calcEdgeCouple2d();
  }
  else if (dimension == 3)
  {
    calcEdgeCouple3d();
  }
  else
  {
    dsAssert(false, "UNEXPECTED");
  }
}

void EdgeCouple::calcEdgeCouple2d() const
{
  ConstTriangleEdgeModelPtr eec = GetRegion().GetTriangleEdgeModel("ElementEdgeCouple");
  dsAssert(eec.get(), "ElementEdgeCouple missing");

  std::vector<double> ev = eec->GetValuesOnEdges();
  SetValues(ev);
}

void EdgeCouple::calcEdgeCouple3d() const
{
  ConstTetrahedronEdgeModelPtr eec = GetRegion().GetTetrahedronEdgeModel("ElementEdgeCouple");
  dsAssert(eec.get(), "ElementEdgeCouple missing");
  std::vector<double> ev = eec->GetValuesOnEdges();
  SetValues(ev);

}

void EdgeCouple::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

