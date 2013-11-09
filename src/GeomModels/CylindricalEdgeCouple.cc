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

#include "CylindricalEdgeCouple.hh"
#include "TriangleEdgeModel.hh"
#include "Region.hh"
#include "dsAssert.hh"

CylindricalEdgeCouple::CylindricalEdgeCouple(RegionPtr rp) :
EdgeModel("CylindricalEdgeCouple", rp, EdgeModel::SCALAR)
{
  const size_t dimension = rp->GetDimension();
  dsAssert(dimension == 2, "CylindricalEdgeCouple 2d Only");
  if (dimension == 2)
  {
    RegisterCallback("ElementCylindricalEdgeCouple");
  }
}


void CylindricalEdgeCouple::calcEdgeScalarValues() const
{
  const size_t dimension=GetRegion().GetDimension();

  if (dimension == 1)
  {
    dsAssert(false, "CylindricalEdgeCouple not supported in 1d");
  }
  else if (dimension == 2)
  {
    calcCylindricalEdgeCouple2d();
  }
  else if (dimension == 3)
  {
    dsAssert(false, "CylindricalEdgeCouple not supported in 3d");
  }
  else
  {
    dsAssert(false, "UNEXPECTED");
  }
}

void CylindricalEdgeCouple::calcCylindricalEdgeCouple2d() const
{
  ConstTriangleEdgeModelPtr eec = GetRegion().GetTriangleEdgeModel("ElementCylindricalEdgeCouple");
  dsAssert(eec.get(), "ElementCylindricalEdgeCouple missing");

  std::vector<double> ev = eec->GetValuesOnEdges();
  SetValues(ev);
}


void CylindricalEdgeCouple::Serialize(std::ostream &of) const
{
  of << "DATAPARENT \"ElementCylindricalEdgeCouple\"";
}

