/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "UnitVec.hh"
#include "Region.hh"
#include "Node.hh"
#include "Edge.hh"
#include "Vector.hh"
#include "EdgeSubModel.hh"
#include "dsAssert.hh"

template <typename DoubleType>
UnitVec<DoubleType>::~UnitVec()
{
}

template <typename DoubleType>
UnitVec<DoubleType>::UnitVec(RegionPtr rp) :
EdgeModel("unitx", rp, EdgeModel::DisplayType::SCALAR)
{
    if (rp->GetDimension() > 1)
    {
        unity = EdgeSubModel<DoubleType>::CreateEdgeSubModel("unity", rp, EdgeModel::DisplayType::SCALAR, this->GetSelfPtr());
    }

    if (rp->GetDimension() > 2)
    {
        unitz = EdgeSubModel<DoubleType>::CreateEdgeSubModel("unitz", rp, EdgeModel::DisplayType::SCALAR, this->GetSelfPtr());
    }
}


template <typename DoubleType>
void UnitVec<DoubleType>::calcEdgeScalarValues() const
{
  const ConstEdgeList &el = GetRegion().GetEdgeList();
  std::vector<DoubleType> ux(el.size());
  std::vector<DoubleType> uy(el.size());
  std::vector<DoubleType> uz(el.size());

  for (size_t i = 0; i < el.size(); ++i)
  {
     const Vector<DoubleType> &v = calcUnitVec(el[i]);
     ux[i] = v.Getx();
     uy[i] = v.Gety();
     uz[i] = v.Getz();
  }

  SetValues(ux);
  if (!unity.expired())
  {
    unity.lock()->SetValues(uy);
  }
  if (!unitz.expired())
  {
    unitz.lock()->SetValues(uz);
  }
}

template <typename DoubleType>
Vector<DoubleType> UnitVec<DoubleType>::calcUnitVec(ConstEdgePtr ep) const
{
    const auto &h0 = ep->GetHead()->Position();
    const auto &h1 = ep->GetTail()->Position();


    Vector<DoubleType> vm(h1.Getx(), h1.Gety(), h1.Getz());
    vm -= Vector<DoubleType>(h0.Getx(), h0.Gety(), h0.Getz());
    vm /= vm.magnitude();

    return vm;
}

template <typename DoubleType>
void UnitVec<DoubleType>::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

template class UnitVec<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class UnitVec<float128>;
#endif

