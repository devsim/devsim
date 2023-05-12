/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "TriangleNodeVolume.hh"
#include "dsAssert.hh"
#include "TriangleEdgeScalarData.hh"
#include "Region.hh"
#include "EdgeModel.hh"
#include "Edge.hh"

template <typename DoubleType>
TriangleNodeVolume<DoubleType>::TriangleNodeVolume(RegionPtr rp)
    : TriangleEdgeModel("ElementNodeVolume", rp, TriangleEdgeModel::DisplayType::SCALAR)
{
    RegisterCallback("EdgeLength");
    RegisterCallback("ElementEdgeCouple");
}

template <typename DoubleType>
void TriangleNodeVolume<DoubleType>::calcTriangleEdgeScalarValues() const
{
  const Region &r = GetRegion();
  const size_t dimension = r.GetDimension();

  ConstTriangleEdgeModelPtr eec = r.GetTriangleEdgeModel("ElementEdgeCouple");
  dsAssert(eec.get(), "UNEXPECTED");

  ConstEdgeModelPtr elen = r.GetEdgeModel("EdgeLength");
  dsAssert(elen.get(), "UNEXPECTED");

  const EdgeScalarList<DoubleType> elens = elen->GetScalarValues<DoubleType>();

  TriangleEdgeScalarData<DoubleType> evol = TriangleEdgeScalarData<DoubleType>(*eec);

  dsAssert(dimension == 2, "UNEXPECTED");

  //// 0.5 * edgecouple * (0.5 * edgelength)
  evol.times_equal_scalar(0.25);

  const ConstTriangleList &tl = GetRegion().GetTriangleList();
  std::vector<DoubleType> ev(3 * tl.size());

  for (size_t tindex = 0; tindex < tl.size(); ++tindex)
  {

    const Region::TriangleToConstEdgeList_t &ttelist = GetRegion().GetTriangleToEdgeList();
    const ConstEdgeList &edgeList = ttelist[tindex];

    /// teindex is 0,1,2
    for (size_t teindex = 0; teindex < edgeList.size(); ++teindex)
    {
      DoubleType vol = elens[edgeList[teindex]->GetIndex()];
      //// TODO: get rid of this messy indexing scheme
      vol *= evol[3*tindex + teindex];

      const size_t oindex = 3*tindex + teindex;
      /// The symmetry that both nodes on an element edge have the same volume may not apply in 3D
      ev[oindex] = vol;
    }
  }

  SetValues(ev);
}

template <typename DoubleType>
void TriangleNodeVolume<DoubleType>::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

template class TriangleNodeVolume<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class TriangleNodeVolume<float128>;
#endif

