/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "TetrahedronEdgeFromEdgeModel.hh"
#include "TetrahedronEdgeSubModel.hh"
#include "Region.hh"
#include "TetrahedronElementField.hh"
#include "dsAssert.hh"
#include "Vector.hh"



template <typename DoubleType>
TetrahedronEdgeFromEdgeModel<DoubleType>::TetrahedronEdgeFromEdgeModel(const std::string &edgemodel, RegionPtr rp)
    : TetrahedronEdgeModel(edgemodel + "_x", rp, TetrahedronEdgeModel::DisplayType::SCALAR), edgeModelName(edgemodel), y_ModelName(edgeModelName+ "_y"), z_ModelName(edgeModelName + "_z")
{
  RegisterCallback(edgemodel);
  new TetrahedronEdgeSubModel<DoubleType>(y_ModelName, rp, TetrahedronEdgeModel::DisplayType::SCALAR, this->GetSelfPtr());
  new TetrahedronEdgeSubModel<DoubleType>(z_ModelName, rp, TetrahedronEdgeModel::DisplayType::SCALAR, this->GetSelfPtr());
}

//// Need to figure out the deleter situation from sub models
//// Perhaps a Delete SubModels method??????

template <typename DoubleType>
void TetrahedronEdgeFromEdgeModel<DoubleType>::calcTetrahedronEdgeScalarValues() const
{
  const Region &reg = GetRegion();

  const ConstEdgeModelPtr emp = reg.GetEdgeModel(edgeModelName);
  dsAssert(emp.get(), "UNEXPECTED");

  const ConstTetrahedronEdgeModelPtr tempy = reg.GetTetrahedronEdgeModel(y_ModelName);
  dsAssert(tempy.get(), "UNEXPECTED");

  const ConstTetrahedronEdgeModelPtr tempz = reg.GetTetrahedronEdgeModel(z_ModelName);
  dsAssert(tempz.get(), "UNEXPECTED");

  const ConstTetrahedronList &tl = GetRegion().GetTetrahedronList();

  std::vector<DoubleType> evx(6*tl.size());
  std::vector<DoubleType> evy(6*tl.size());
  std::vector<DoubleType> evz(6*tl.size());

  const TetrahedronElementField<DoubleType> &efield = reg.GetTetrahedronElementField<DoubleType>();
  typename TetrahedronElementField<DoubleType>::EdgeVectors_t v;
  for (size_t i = 0; i < tl.size(); ++i)
  {
    const Tetrahedron &tetrahedron = *tl[i];
    efield.GetTetrahedronElementField(tetrahedron, *emp, v);
    for (size_t j = 0; j < 6; ++j)
    {
      const Vector<DoubleType> &vec = v[j];
      evx[6*i + j] = vec.Getx();
      evy[6*i + j] = vec.Gety();
      evz[6*i + j] = vec.Getz();
    }
  }

  SetValues(evx);
  std::const_pointer_cast<TetrahedronEdgeModel, const TetrahedronEdgeModel>(tempy)->SetValues(evy);
  std::const_pointer_cast<TetrahedronEdgeModel, const TetrahedronEdgeModel>(tempz)->SetValues(evz);
}

template <typename DoubleType>
void TetrahedronEdgeFromEdgeModel<DoubleType>::Serialize(std::ostream &of) const
{
  of << "COMMAND element_from_edge_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -edge_model \"" << edgeModelName << "\"";
}

template class TetrahedronEdgeFromEdgeModel<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class TetrahedronEdgeFromEdgeModel<float128>;
#endif


TetrahedronEdgeModelPtr CreateTetrahedronEdgeFromEdgeModel(const std::string &edgemodel, RegionPtr rp)
{
  const bool use_extended = rp->UseExtendedPrecisionModels();
  return create_tetrahedron_edge_model<TetrahedronEdgeFromEdgeModel<double>, TetrahedronEdgeFromEdgeModel<extended_type>>(use_extended, edgemodel, rp);
}




