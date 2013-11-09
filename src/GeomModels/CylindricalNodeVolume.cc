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

#include "CylindricalNodeVolume.hh"
#include "Region.hh"
#include "dsAssert.hh"
#include "TriangleEdgeModel.hh"
#include "Node.hh"
#include "Edge.hh"

CylindricalNodeVolume::CylindricalNodeVolume(RegionPtr rp)
    : NodeModel("CylindricalNodeVolume", rp, NodeModel::SCALAR)
{
    const size_t dimension = rp->GetDimension();
    dsAssert(dimension == 2, "CylindricalNodeVolume 2d Only");

    if (dimension == 2)
    {
      RegisterCallback("ElementCylindricalNodeVolume@en0");
      RegisterCallback("ElementCylindricalNodeVolume@en1");
    }
}

void CylindricalNodeVolume::calcNodeScalarValues() const
{
  const Region &r = GetRegion();
  const size_t dimension = r.GetDimension();

  dsAssert(dimension == 2, "CylindricalNodeVolume 2d Only");

  std::vector<double> nv(r.GetNumberNodes());


  if (dimension == 2)
  {
    const Region &region = GetRegion();

    ConstTriangleEdgeModelPtr eec0 = region.GetTriangleEdgeModel("ElementCylindricalNodeVolume@en0");
    ConstTriangleEdgeModelPtr eec1 = region.GetTriangleEdgeModel("ElementCylindricalNodeVolume@en1");

    dsAssert(eec0.get(), "ElementNodeVolume@en0 missing");
    dsAssert(eec1.get(), "ElementNodeVolume@en1 missing");

    const EdgeScalarList &nv0 = eec0->GetValuesOnEdges();
    const EdgeScalarList &nv1 = eec1->GetValuesOnEdges();

    const ConstEdgeList &edge_list = region.GetEdgeList();

    dsAssert(nv0.size() == nv1.size(), "UNEXPECTED");
    dsAssert(nv0.size() == edge_list.size(), "UNEXPECTED");

    for (size_t i = 0; i < edge_list.size(); ++i)
    {
      const Edge &edge  = *(edge_list[i]);
      const size_t ni0 = edge.GetHead()->GetIndex();
      const size_t ni1 = edge.GetTail()->GetIndex();

      nv[ni0] += nv0[i];
      nv[ni1] += nv1[i];

    }
  }
  else
  {
    dsAssert(false, "UNEXPECTED");
  }

  SetValues(nv);
}

void CylindricalNodeVolume::setInitialValues()
{
    DefaultInitializeValues();
}

void CylindricalNodeVolume::Serialize(std::ostream &of) const
{
  of << "DATAPARENT \"ElementCylindricalNodeVolume@en0\"";
}

