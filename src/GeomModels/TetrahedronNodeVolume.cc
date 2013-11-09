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

#include "TetrahedronNodeVolume.hh"
#include "dsAssert.hh"
#include "TetrahedronEdgeScalarData.hh"
#include "Region.hh"
#include "EdgeModel.hh"
#include "EdgeData.hh"
#include "Edge.hh"

TetrahedronNodeVolume::TetrahedronNodeVolume(RegionPtr rp)
    : TetrahedronEdgeModel("ElementNodeVolume", rp, TetrahedronEdgeModel::SCALAR)
{
    ///// 1/3 * base area time perpendicular distance to 3rd node
    ///// 1./3. * (0.5 * EdgeLength) * ElementEdgeCouple
    RegisterCallback("EdgeLength");
    RegisterCallback("ElementEdgeCouple");
}

void TetrahedronNodeVolume::calcTetrahedronEdgeScalarValues() const
{
  const Region &r = GetRegion();
//  const size_t dimension = r.GetDimension();

  ConstTetrahedronEdgeModelPtr eec = r.GetTetrahedronEdgeModel("ElementEdgeCouple");
  dsAssert(eec.get(), "UNEXPECTED");

  ConstEdgeModelPtr elen = r.GetEdgeModel("EdgeLength");
  dsAssert(elen.get(), "UNEXPECTED");

  TetrahedronEdgeScalarData evol = TetrahedronEdgeScalarData(*eec);


  //// 1./3. * edgecouple * (0.5 * edgelength)
  evol *= (1.0/6.0);

  const EdgeScalarList &edge_lengths = elen->GetScalarValues();

  const ConstTetrahedronList &tl = GetRegion().GetTetrahedronList();
  std::vector<double> ev(6 * tl.size());


  const Region::TetrahedronToConstEdgeDataList_t &ttelist = GetRegion().GetTetrahedronToEdgeDataList();
  for (size_t tindex = 0; tindex < tl.size(); ++tindex)
  {
    const ConstEdgeDataList &edgeDataList = ttelist[tindex];

    /// teindex is 0,1,2,3,4,5
//    dsAssert(edgeDataList.size() == 6, "UNEXPECTED");
    for (size_t teindex = 0; teindex < edgeDataList.size(); ++teindex)
    {
      double vol = edge_lengths[edgeDataList[teindex]->edge->GetIndex()];

      const size_t oindex = 6*tindex + teindex;

      vol *= evol[oindex];
      ev[oindex] = vol;
    }
  }
  SetValues(ev);
}

void TetrahedronNodeVolume::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

