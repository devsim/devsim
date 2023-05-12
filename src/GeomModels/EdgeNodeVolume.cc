/***
DEVSIM
Copyright 2021 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "EdgeNodeVolume.hh"
#include "Region.hh"
#include "Edge.hh"
#include "dsAssert.hh"
#include "TriangleEdgeModel.hh"
#include "TetrahedronEdgeModel.hh"
#include "EdgeScalarData.hh"

template <typename DoubleType>
EdgeNodeVolume<DoubleType>::EdgeNodeVolume(RegionPtr rp) :
EdgeModel("EdgeNodeVolume", rp, EdgeModel::DisplayType::SCALAR)
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


template <typename DoubleType>
void EdgeNodeVolume<DoubleType>::calcEdgeScalarValues() const
{
  const Region &r = GetRegion();
  const size_t dimension = r.GetDimension();

  if (dimension == 1)
  {
    ConstEdgeModelPtr ec = r.GetEdgeModel("EdgeCouple");
    dsAssert(ec.get(), "UNEXPECTED");
    ConstEdgeModelPtr elen = r.GetEdgeModel("EdgeLength");
    dsAssert(elen.get(), "UNEXPECTED");

    EdgeScalarData<DoubleType> evol = EdgeScalarData<DoubleType>(*ec);
    evol.times_equal_model(*elen);

    //// valid only for dimension == 1
    evol.times_equal_scalar(0.5);
    SetValues(evol.GetScalarList());
  }
  else if (dimension == 2)
  {
    calcEdgeNodeVolume2d();
  }
  else if (dimension == 3)
  {
    calcEdgeNodeVolume3d();
  }
  else
  {
    dsAssert(false, "UNEXPECTED");
  }
}

template <typename DoubleType>
void EdgeNodeVolume<DoubleType>::calcEdgeNodeVolume2d() const
{
  ConstTriangleEdgeModelPtr eec = GetRegion().GetTriangleEdgeModel("ElementNodeVolume");
  dsAssert(eec.get(), "ElementNodeVolume missing");

  std::vector<DoubleType> ev = eec->GetValuesOnEdges<DoubleType>();
  SetValues(ev);
}

template <typename DoubleType>
void EdgeNodeVolume<DoubleType>::calcEdgeNodeVolume3d() const
{
  ConstTetrahedronEdgeModelPtr eec = GetRegion().GetTetrahedronEdgeModel("ElementNodeVolume");
  dsAssert(eec.get(), "ElementNodeVolume missing");
  std::vector<DoubleType> ev = eec->GetValuesOnEdges<DoubleType>();
  SetValues(ev);
}

template <typename DoubleType>
void EdgeNodeVolume<DoubleType>::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

template class EdgeNodeVolume<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class EdgeNodeVolume<float128>;
#endif


