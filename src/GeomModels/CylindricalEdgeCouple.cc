/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "CylindricalEdgeCouple.hh"
#include "TriangleEdgeModel.hh"
#include "Region.hh"
#include "dsAssert.hh"

template <typename DoubleType>
CylindricalEdgeCouple<DoubleType>::CylindricalEdgeCouple(RegionPtr rp) :
EdgeModel("CylindricalEdgeCouple", rp, EdgeModel::DisplayType::SCALAR)
{
  const size_t dimension = rp->GetDimension();
  dsAssert(dimension == 2, "CylindricalEdgeCouple 2d Only");
  if (dimension == 2)
  {
    RegisterCallback("ElementCylindricalEdgeCouple");
  }
}


template <typename DoubleType>
void CylindricalEdgeCouple<DoubleType>::calcEdgeScalarValues() const
{
  const size_t dimension=GetRegion().GetDimension();

  if (dimension == 1)
  {
    dsAssert(false, "CylindricalEdgeCouple not supported in 1d");
  }
  else if (dimension == 2)
  {
    calcCylindricalEdgeCouple2d();
  }
  else if (dimension == 3)
  {
    dsAssert(false, "CylindricalEdgeCouple not supported in 3d");
  }
  else
  {
    dsAssert(false, "UNEXPECTED");
  }
}

template <typename DoubleType>
void CylindricalEdgeCouple<DoubleType>::calcCylindricalEdgeCouple2d() const
{
  ConstTriangleEdgeModelPtr eec = GetRegion().GetTriangleEdgeModel("ElementCylindricalEdgeCouple");
  dsAssert(eec.get(), "ElementCylindricalEdgeCouple missing");

  std::vector<DoubleType> ev = eec->GetValuesOnEdges<DoubleType>();
  SetValues(ev);
}


template <typename DoubleType>
void CylindricalEdgeCouple<DoubleType>::Serialize(std::ostream &of) const
{
  of << "DATAPARENT \"ElementCylindricalEdgeCouple\"";
}

template class CylindricalEdgeCouple<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class CylindricalEdgeCouple<float128>;
#endif

EdgeModelPtr CreateCylindricalEdgeCouple(RegionPtr rp)
{
  const bool use_extended = rp->UseExtendedPrecisionModels();
  return create_edge_model<CylindricalEdgeCouple<double>, CylindricalEdgeCouple<extended_type>>(use_extended, rp);
}

