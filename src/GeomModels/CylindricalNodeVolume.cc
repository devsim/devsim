/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "CylindricalNodeVolume.hh"
#include "Region.hh"
#include "dsAssert.hh"
#include "TriangleEdgeModel.hh"
#include "Node.hh"
#include "Edge.hh"

template <typename DoubleType>
CylindricalNodeVolume<DoubleType>::CylindricalNodeVolume(RegionPtr rp)
    : NodeModel("CylindricalNodeVolume", rp, NodeModel::DisplayType::SCALAR)
{
    const size_t dimension = rp->GetDimension();
    dsAssert(dimension == 2, "CylindricalNodeVolume 2d Only");

    if (dimension == 2)
    {
      RegisterCallback("ElementCylindricalNodeVolume@en0");
      RegisterCallback("ElementCylindricalNodeVolume@en1");
    }
}

template <typename DoubleType>
void CylindricalNodeVolume<DoubleType>::calcNodeScalarValues() const
{
  const Region &r = GetRegion();
  const size_t dimension = r.GetDimension();

  dsAssert(dimension == 2, "CylindricalNodeVolume 2d Only");

  std::vector<DoubleType> nv(r.GetNumberNodes());


  if (dimension == 2)
  {
    const Region &region = GetRegion();

    ConstTriangleEdgeModelPtr eec0 = region.GetTriangleEdgeModel("ElementCylindricalNodeVolume@en0");
    ConstTriangleEdgeModelPtr eec1 = region.GetTriangleEdgeModel("ElementCylindricalNodeVolume@en1");

    dsAssert(eec0.get(), "ElementNodeVolume@en0 missing");
    dsAssert(eec1.get(), "ElementNodeVolume@en1 missing");

    const EdgeScalarList<DoubleType> &nv0 = eec0->GetValuesOnEdges<DoubleType>();
    const EdgeScalarList<DoubleType> &nv1 = eec1->GetValuesOnEdges<DoubleType>();

    const ConstEdgeList &edge_list = region.GetEdgeList();

    dsAssert(nv0.size() == nv1.size(), "UNEXPECTED");
    dsAssert(nv0.size() == edge_list.size(), "UNEXPECTED");

    for (size_t i = 0; i < edge_list.size(); ++i)
    {
      const Edge &edge  = *(edge_list[i]);
      const size_t ni0 = edge.GetHead()->GetIndex();
      const size_t ni1 = edge.GetTail()->GetIndex();

      nv[ni0] += nv0[i];
      nv[ni1] += nv1[i];

    }
  }
  else
  {
    dsAssert(false, "UNEXPECTED");
  }

  SetValues(nv);
}

template <typename DoubleType>
void CylindricalNodeVolume<DoubleType>::setInitialValues()
{
    DefaultInitializeValues();
}

template <typename DoubleType>
void CylindricalNodeVolume<DoubleType>::Serialize(std::ostream &of) const
{
  of << "DATAPARENT \"ElementCylindricalNodeVolume@en0\"";
}

template class CylindricalNodeVolume<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class CylindricalNodeVolume<float128>;
#endif

NodeModelPtr CreateCylindricalNodeVolume(RegionPtr rp)
{
  const bool use_extended = rp->UseExtendedPrecisionModels();
  return create_node_model<CylindricalNodeVolume<double>, CylindricalNodeVolume<extended_type>>(use_extended, rp);
}


