/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "TriangleEdgeFromEdgeModel.hh"
#include "TriangleEdgeSubModel.hh"
#include "Region.hh"
#include "TriangleElementField.hh"
#include "dsAssert.hh"
#include "Vector.hh"


template <typename DoubleType>
TriangleEdgeFromEdgeModel<DoubleType>::TriangleEdgeFromEdgeModel(const std::string &edgemodel, RegionPtr rp)
    : TriangleEdgeModel(edgemodel + "_x", rp, TriangleEdgeModel::DisplayType::SCALAR), edgeModelName(edgemodel), y_ModelName(edgeModelName + "_y")
{
  RegisterCallback(edgemodel);
  new TriangleEdgeSubModel<DoubleType>(y_ModelName, rp, TriangleEdgeModel::DisplayType::SCALAR, this->GetSelfPtr());
}

//// Need to figure out the deleter situation from sub models
//// Perhaps a Delete SubModels method??????

template <typename DoubleType>
void TriangleEdgeFromEdgeModel<DoubleType>::calcTriangleEdgeScalarValues() const
{
  const Region &reg = GetRegion();

  const ConstEdgeModelPtr emp = reg.GetEdgeModel(edgeModelName);
  dsAssert(emp.get(), "UNEXPECTED");

  const ConstTriangleEdgeModelPtr tempy = reg.GetTriangleEdgeModel(y_ModelName);
  dsAssert(tempy.get(), "UNEXPECTED");

  const ConstTriangleEdgeModelPtr eec = reg.GetTriangleEdgeModel("ElementEdgeCouple");
  dsAssert(eec.get(), "UNEXPECTED");

  const ConstTriangleList &tl = GetRegion().GetTriangleList();

  std::vector<DoubleType> evx(3*tl.size());
  std::vector<DoubleType> evy(3*tl.size());

  const TriangleElementField<DoubleType> &efield = reg.GetTriangleElementField<DoubleType>();
  typename TriangleElementField<DoubleType>::EdgeVectors_t v;
  for (size_t i = 0; i < tl.size(); ++i)
  {
    const Triangle &triangle = *tl[i];
    efield.GetTriangleElementField(triangle, *eec, *emp, v);
    for (size_t j = 0; j < 3; ++j)
    {
      evx[3*i + j] = v[j].Getx();
      evy[3*i + j] = v[j].Gety();
    }
  }

  SetValues(evx);
  std::const_pointer_cast<TriangleEdgeModel, const TriangleEdgeModel>(tempy)->SetValues(evy);
}

template <typename DoubleType>
void TriangleEdgeFromEdgeModel<DoubleType>::Serialize(std::ostream &of) const
{
  of << "COMMAND element_from_edge_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -edge_model \"" << edgeModelName << "\"";
}

template class TriangleEdgeFromEdgeModel<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class TriangleEdgeFromEdgeModel<float128>;
#endif


TriangleEdgeModelPtr CreateTriangleEdgeFromEdgeModel(const std::string &edgemodel, RegionPtr rp)
{
  const bool use_extended = rp->UseExtendedPrecisionModels();
  return create_triangle_edge_model<TriangleEdgeFromEdgeModel<double>, TriangleEdgeFromEdgeModel<extended_type>>(use_extended, edgemodel, rp);
}



