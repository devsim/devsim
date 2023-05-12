/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "EdgeCouple.hh"
#include "Region.hh"
#include "Edge.hh"
#include "dsAssert.hh"
#include "TriangleEdgeModel.hh"
#include "TetrahedronEdgeModel.hh"

template <typename DoubleType>
EdgeCouple<DoubleType>::EdgeCouple(RegionPtr rp) :
EdgeModel("EdgeCouple", rp, EdgeModel::DisplayType::SCALAR)
{
  const size_t dimension = rp->GetDimension();
  if ((dimension == 2) || (dimension == 3))
  {
    RegisterCallback("ElementEdgeCouple");
  }
}


template <typename DoubleType>
void EdgeCouple<DoubleType>::calcEdgeScalarValues() const
{
  const size_t dimension=GetRegion().GetDimension();

  if (dimension == 1)
  {
    std::vector<DoubleType> ev(GetRegion().GetNumberEdges(), 1.0);
    SetValues(ev);
  }
  else if (dimension == 2)
  {
    calcEdgeCouple2d();
  }
  else if (dimension == 3)
  {
    calcEdgeCouple3d();
  }
  else
  {
    dsAssert(false, "UNEXPECTED");
  }
}

template <typename DoubleType>
void EdgeCouple<DoubleType>::calcEdgeCouple2d() const
{
  ConstTriangleEdgeModelPtr eec = GetRegion().GetTriangleEdgeModel("ElementEdgeCouple");
  dsAssert(eec.get(), "ElementEdgeCouple missing");

  std::vector<DoubleType> ev = eec->GetValuesOnEdges<DoubleType>();
  SetValues(ev);
}

template <typename DoubleType>
void EdgeCouple<DoubleType>::calcEdgeCouple3d() const
{
  ConstTetrahedronEdgeModelPtr eec = GetRegion().GetTetrahedronEdgeModel("ElementEdgeCouple");
  dsAssert(eec.get(), "ElementEdgeCouple missing");
  std::vector<DoubleType> ev = eec->GetValuesOnEdges<DoubleType>();
  SetValues(ev);

}

template <typename DoubleType>
void EdgeCouple<DoubleType>::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

template class EdgeCouple<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class EdgeCouple<float128>;
#endif


