/***
DEVSIM
Copyright 2013 DEVSIM LLC

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

#include "TetrahedronNodeVolume.hh"
#include "dsAssert.hh"
#include "TetrahedronEdgeScalarData.hh"
#include "Region.hh"
#include "EdgeModel.hh"
#include "EdgeData.hh"
#include "Edge.hh"

template <typename DoubleType>
TetrahedronNodeVolume<DoubleType>::TetrahedronNodeVolume(RegionPtr rp)
    : TetrahedronEdgeModel("ElementNodeVolume", rp, TetrahedronEdgeModel::DisplayType::SCALAR)
{
    ///// 1/3 * base area time perpendicular distance to 3rd node
    ///// 1./3. * (0.5 * EdgeLength) * ElementEdgeCouple
    RegisterCallback("EdgeLength");
    RegisterCallback("ElementEdgeCouple");
}

template <typename DoubleType>
void TetrahedronNodeVolume<DoubleType>::calcTetrahedronEdgeScalarValues() const
{
  const Region &r = GetRegion();
//  const size_t dimension = r.GetDimension();

  ConstTetrahedronEdgeModelPtr eec = r.GetTetrahedronEdgeModel("ElementEdgeCouple");
  dsAssert(eec.get(), "UNEXPECTED");

  ConstEdgeModelPtr elen = r.GetEdgeModel("EdgeLength");
  dsAssert(elen.get(), "UNEXPECTED");

  TetrahedronEdgeScalarData<DoubleType> evol = TetrahedronEdgeScalarData<DoubleType>(*eec);


  //// 1./3. * edgecouple * (0.5 * edgelength)
  evol.times_equal_scalar(static_cast<DoubleType>(1.0)/static_cast<DoubleType>(6.0));

  const EdgeScalarList<DoubleType> &edge_lengths = elen->GetScalarValues<DoubleType>();

  const ConstTetrahedronList &tl = GetRegion().GetTetrahedronList();
  std::vector<DoubleType> ev(6 * tl.size());


  const Region::TetrahedronToConstEdgeDataList_t &ttelist = GetRegion().GetTetrahedronToEdgeDataList();
  for (size_t tindex = 0; tindex < tl.size(); ++tindex)
  {
    const ConstEdgeDataList &edgeDataList = ttelist[tindex];

    /// teindex is 0,1,2,3,4,5
//    dsAssert(edgeDataList.size() == 6, "UNEXPECTED");
    for (size_t teindex = 0; teindex < edgeDataList.size(); ++teindex)
    {
      DoubleType vol = edge_lengths[edgeDataList[teindex]->edge->GetIndex()];

      const size_t oindex = 6*tindex + teindex;

      vol *= evol[oindex];
      ev[oindex] = vol;
    }
  }
  SetValues(ev);
}

template <typename DoubleType>
void TetrahedronNodeVolume<DoubleType>::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

template class TetrahedronNodeVolume<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class TetrahedronNodeVolume<float128>;
#endif

