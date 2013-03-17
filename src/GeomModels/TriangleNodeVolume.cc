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

#include "TriangleNodeVolume.hh"
#include "dsAssert.hh"
#include "TriangleEdgeScalarData.hh"
#include "Region.hh"
#include "EdgeModel.hh"
#include "Edge.hh"

TriangleNodeVolume::TriangleNodeVolume(RegionPtr rp)
    : TriangleEdgeModel("ElementNodeVolume", rp, TriangleEdgeModel::SCALAR)
{
    RegisterCallback("EdgeLength");
    RegisterCallback("ElementEdgeCouple");
}

void TriangleNodeVolume::calcTriangleEdgeScalarValues() const
{
  const Region &r = GetRegion();
  const size_t dimension = r.GetDimension();

  ConstTriangleEdgeModelPtr eec = r.GetTriangleEdgeModel("ElementEdgeCouple");
  dsAssert(eec != NULL, "UNEXPECTED");

  ConstEdgeModelPtr elen = r.GetEdgeModel("EdgeLength");
  dsAssert(elen != NULL, "UNEXPECTED");

  const EdgeScalarList elens = elen->GetScalarValues();

  TriangleEdgeScalarData evol = TriangleEdgeScalarData(*eec);

  dsAssert(dimension == 2, "UNEXPECTED");

  //// 0.5 * edgecouple * (0.5 * edgelength)
  evol *= 0.25;

  const ConstTriangleList &tl = GetRegion().GetTriangleList();
  std::vector<double> ev(3 * tl.size());

  for (size_t tindex = 0; tindex < tl.size(); ++tindex)
  {

    const Region::TriangleToConstEdgeList_t &ttelist = GetRegion().GetTriangleToEdgeList();
    const ConstEdgeList &edgeList = ttelist[tindex];

    /// teindex is 0,1,2
    for (size_t teindex = 0; teindex < edgeList.size(); ++teindex)
    {
      double vol = elens[edgeList[teindex]->GetIndex()];
      //// TODO: get rid of this messy indexing scheme
      vol *= evol[3*tindex + teindex];

      const size_t oindex = 3*tindex + teindex;
      /// The symmetry that both nodes on an element edge have the same volume may not apply in 3D
      ev[oindex] = vol;
    }
  }

  SetValues(ev);
}

void TriangleNodeVolume::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

