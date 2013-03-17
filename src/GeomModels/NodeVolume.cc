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

#include "NodeVolume.hh"
#include "Region.hh"
#include "Edge.hh"
#include "EdgeModel.hh"
#include "EdgeScalarData.hh"
#include "dsAssert.hh"
#include "TriangleEdgeModel.hh"
#include "TetrahedronEdgeModel.hh"

NodeVolume::NodeVolume(RegionPtr rp)
    : NodeModel("NodeVolume", rp, NodeModel::SCALAR)
{
    const size_t dimension = rp->GetDimension();

    if (dimension == 1)
    {
      RegisterCallback("EdgeLength");
      RegisterCallback("EdgeCouple");
    }
    else if ((dimension == 2) || (dimension == 3))
    {
      RegisterCallback("ElementNodeVolume");
    }
}

void NodeVolume::calcNodeScalarValues() const
{
  const Region &r = GetRegion();
  const size_t dimension = r.GetDimension();

  std::vector<double> nv(r.GetNumberNodes());

  if (dimension == 1)
  {
    ConstEdgeModelPtr ec = r.GetEdgeModel("EdgeCouple");
    dsAssert(ec, "UNEXPECTED");
    ConstEdgeModelPtr elen = r.GetEdgeModel("EdgeLength");
    dsAssert(elen, "UNEXPECTED");

    EdgeScalarData evol = EdgeScalarData(*ec);
    evol *= *elen;

    //// valid only for dimension == 1
    evol *= 0.5;

    for (size_t i = 0; i < nv.size(); ++i)
    {
      double volume = 0.0;

      const ConstEdgeList &el = r.GetNodeToEdgeList()[i];

      ConstEdgeList::const_iterator it = el.begin();
      const ConstEdgeList::const_iterator itend = el.end();
      for ( ; it != itend; ++it)
      {
        volume += evol[(*it)->GetIndex()]; 
      }

      nv[i] = volume;
    }
  }
  else if (dimension == 2)
  {
    ConstTriangleEdgeModelPtr eec = GetRegion().GetTriangleEdgeModel("ElementNodeVolume");
    dsAssert(eec, "ElementNodeVolume missing");

    eec->GetScalarValuesOnNodes(TriangleEdgeModel::SUM, nv);
  }
  else if (dimension == 3)
  {
    ConstTetrahedronEdgeModelPtr eec = GetRegion().GetTetrahedronEdgeModel("ElementNodeVolume");
    dsAssert(eec, "UNEXPECTED");
    eec->GetScalarValuesOnNodes(TetrahedronEdgeModel::SUM, nv);
  }
  else
  {
    dsAssert(false, "UNEXPECTED");
  }

  SetValues(nv);
}

void NodeVolume::setInitialValues()
{
    DefaultInitializeValues();
}

void NodeVolume::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

