/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "CylindricalEdgeNodeVolume.hh"
#include "Region.hh"
#include "dsAssert.hh"
#include "TriangleEdgeModel.hh"
#include "Edge.hh"
#include "EdgeSubModel.hh"

template <typename DoubleType>
CylindricalEdgeNodeVolume<DoubleType>::CylindricalEdgeNodeVolume(RegionPtr rp)
    : EdgeModel("CylindricalEdgeNodeVolume@n0", rp, EdgeModel::DisplayType::SCALAR)
{
    const size_t dimension = rp->GetDimension();
    dsAssert(dimension == 2, "CylindricalEdgeNodeVolume 2d Only");

    if (dimension == 2)
    {
      RegisterCallback("ElementCylindricalNodeVolume@en0");
      RegisterCallback("ElementCylindricalNodeVolume@en1");
    }

    node1Volume_ = EdgeSubModel<DoubleType>::CreateEdgeSubModel("CylindricalEdgeNodeVolume@n1", rp, EdgeModel::DisplayType::SCALAR, this->GetSelfPtr());
}

template <typename DoubleType>
void CylindricalEdgeNodeVolume<DoubleType>::calcEdgeScalarValues() const
{
  const Region &r = GetRegion();
  const size_t dimension = r.GetDimension();

  dsAssert(dimension == 2, "CylindricalEdgeNodeVolume 2d Only");

  std::vector<DoubleType> nv(r.GetNumberNodes());


  if (dimension == 2)
  {
    const Region &region = GetRegion();

    ConstTriangleEdgeModelPtr eec0 = region.GetTriangleEdgeModel("ElementCylindricalNodeVolume@en0");
    ConstTriangleEdgeModelPtr eec1 = region.GetTriangleEdgeModel("ElementCylindricalNodeVolume@en1");

    dsAssert(eec0.get(), "ElementCylindricalNodeVolume@en0 missing");
    dsAssert(eec1.get(), "ElementCylindricalNodeVolume@en1 missing");

    const EdgeScalarList<DoubleType> &nv0 = eec0->GetValuesOnEdges<DoubleType>();
    const EdgeScalarList<DoubleType> &nv1 = eec1->GetValuesOnEdges<DoubleType>();

    SetValues(nv0);
    node1Volume_.lock()->SetValues(nv1);

  }
  else
  {
    dsAssert(false, "UNEXPECTED");
  }

  SetValues(nv);
}

template <typename DoubleType>
void CylindricalEdgeNodeVolume<DoubleType>::Serialize(std::ostream &of) const
{
  of << "DATAPARENT \"ElementCylindricalNodeVolume@en0\"";
}

template class CylindricalEdgeNodeVolume<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class CylindricalEdgeNodeVolume<float128>;
#endif

EdgeModelPtr CreateCylindricalEdgeNodeVolume(RegionPtr rp)
{
  const bool use_extended = rp->UseExtendedPrecisionModels();
  return create_edge_model<CylindricalEdgeNodeVolume<double>, CylindricalEdgeNodeVolume<extended_type>>(use_extended, rp);
}


