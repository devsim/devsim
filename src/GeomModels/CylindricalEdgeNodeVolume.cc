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

#include "CylindricalEdgeNodeVolume.hh"
#include "Region.hh"
#include "dsAssert.hh"
#include "TriangleEdgeModel.hh"
#include "Edge.hh"
#include "EdgeSubModel.hh"

CylindricalEdgeNodeVolume::CylindricalEdgeNodeVolume(RegionPtr rp)
    : EdgeModel("CylindricalEdgeNodeVolume@n0", rp, EdgeModel::SCALAR)
{
    const size_t dimension = rp->GetDimension();
    dsAssert(dimension == 2, "CylindricalEdgeNodeVolume 2d Only");

    if (dimension == 2)
    {
      RegisterCallback("ElementCylindricalNodeVolume@en0");
      RegisterCallback("ElementCylindricalNodeVolume@en1");
    }

    node1Volume_ = EdgeSubModel::CreateEdgeSubModel("CylindricalEdgeNodeVolume@n1", rp, EdgeModel::SCALAR, this->GetSelfPtr());
}

void CylindricalEdgeNodeVolume::calcEdgeScalarValues() const
{
  const Region &r = GetRegion();
  const size_t dimension = r.GetDimension();

  dsAssert(dimension == 2, "CylindricalEdgeNodeVolume 2d Only");

  std::vector<double> nv(r.GetNumberNodes());


  if (dimension == 2)
  {
    const Region &region = GetRegion();

    ConstTriangleEdgeModelPtr eec0 = region.GetTriangleEdgeModel("ElementCylindricalNodeVolume@en0");
    ConstTriangleEdgeModelPtr eec1 = region.GetTriangleEdgeModel("ElementCylindricalNodeVolume@en1");

    dsAssert(eec0.get(), "ElementCylindricalNodeVolume@en0 missing");
    dsAssert(eec1.get(), "ElementCylindricalNodeVolume@en1 missing");

    const EdgeScalarList &nv0 = eec0->GetValuesOnEdges();
    const EdgeScalarList &nv1 = eec1->GetValuesOnEdges();

    SetValues(nv0);
    node1Volume_.lock()->SetValues(nv1);

  }
  else
  {
    dsAssert(false, "UNEXPECTED");
  }

  SetValues(nv);
}

void CylindricalEdgeNodeVolume::Serialize(std::ostream &of) const
{
  of << "DATAPARENT \"ElementCylindricalNodeVolume@en0\"";
}

