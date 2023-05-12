/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "TetrahedronEdgeFromEdgeModelDerivative.hh"
#include "TetrahedronEdgeSubModel.hh"
#include "Region.hh"
#include "TetrahedronElementField.hh"
#include "dsAssert.hh"
#include "Vector.hh"

template <typename DoubleType>
TetrahedronEdgeFromEdgeModelDerivative<DoubleType>::TetrahedronEdgeFromEdgeModelDerivative(
        const std::string &edgemodel,
        const std::string &derivative,
        RegionPtr rp
    )
    : TetrahedronEdgeModel(edgemodel + "_x:" + derivative + "@en0", rp, TetrahedronEdgeModel::DisplayType::NODISPLAY),
      edgeModelName(edgemodel),
      nodeModelName(derivative)
{
  const std::string tmx = edgeModelName + "_x";
  const std::string tmy = edgeModelName + "_y";
  const std::string tmz = edgeModelName + "_z";

  const std::string tmpname = edgeModelName + ":" + nodeModelName;
  edgeModelName0 = tmpname + "@n0";
  edgeModelName1 = tmpname + "@n1";

  x_ModelName1 = tmx + ":" + nodeModelName + "@en1";
  x_ModelName2 = tmx + ":" + nodeModelName + "@en2";
  x_ModelName3 = tmx + ":" + nodeModelName + "@en3";
  y_ModelName0 = tmy + ":" + nodeModelName + "@en0";
  y_ModelName1 = tmy + ":" + nodeModelName + "@en1";
  y_ModelName2 = tmy + ":" + nodeModelName + "@en2";
  y_ModelName3 = tmy + ":" + nodeModelName + "@en3";
  z_ModelName0 = tmz + ":" + nodeModelName + "@en0";
  z_ModelName1 = tmz + ":" + nodeModelName + "@en1";
  z_ModelName2 = tmz + ":" + nodeModelName + "@en2";
  z_ModelName3 = tmz + ":" + nodeModelName + "@en3";

  RegisterCallback(edgeModelName0);
  RegisterCallback(edgeModelName1);
  new TetrahedronEdgeSubModel<DoubleType>(x_ModelName1, rp, TetrahedronEdgeModel::DisplayType::NODISPLAY, this->GetSelfPtr());
  new TetrahedronEdgeSubModel<DoubleType>(x_ModelName2, rp, TetrahedronEdgeModel::DisplayType::NODISPLAY, this->GetSelfPtr());
  new TetrahedronEdgeSubModel<DoubleType>(x_ModelName3, rp, TetrahedronEdgeModel::DisplayType::NODISPLAY, this->GetSelfPtr());
  new TetrahedronEdgeSubModel<DoubleType>(y_ModelName0, rp, TetrahedronEdgeModel::DisplayType::NODISPLAY, this->GetSelfPtr());
  new TetrahedronEdgeSubModel<DoubleType>(y_ModelName1, rp, TetrahedronEdgeModel::DisplayType::NODISPLAY, this->GetSelfPtr());
  new TetrahedronEdgeSubModel<DoubleType>(y_ModelName2, rp, TetrahedronEdgeModel::DisplayType::NODISPLAY, this->GetSelfPtr());
  new TetrahedronEdgeSubModel<DoubleType>(y_ModelName3, rp, TetrahedronEdgeModel::DisplayType::NODISPLAY, this->GetSelfPtr());
  new TetrahedronEdgeSubModel<DoubleType>(z_ModelName0, rp, TetrahedronEdgeModel::DisplayType::NODISPLAY, this->GetSelfPtr());
  new TetrahedronEdgeSubModel<DoubleType>(z_ModelName1, rp, TetrahedronEdgeModel::DisplayType::NODISPLAY, this->GetSelfPtr());
  new TetrahedronEdgeSubModel<DoubleType>(z_ModelName2, rp, TetrahedronEdgeModel::DisplayType::NODISPLAY, this->GetSelfPtr());
  new TetrahedronEdgeSubModel<DoubleType>(z_ModelName3, rp, TetrahedronEdgeModel::DisplayType::NODISPLAY, this->GetSelfPtr());
}

//// Need to figure out the deleter situation from sub models
//// Perhaps a Delete SubModels method??????

template <typename DoubleType>
void TetrahedronEdgeFromEdgeModelDerivative<DoubleType>::calcTetrahedronEdgeScalarValues() const
{
  const Region &reg = GetRegion();

  ConstEdgeModelPtr emp[2];
  emp[0] = reg.GetEdgeModel(edgeModelName0);
  emp[1] = reg.GetEdgeModel(edgeModelName1);

  for (size_t i = 0; i < 2; ++i)
  {
    dsAssert(emp[i].get(), "UNEXPECTED");
  }

  ConstTetrahedronEdgeModelPtr tempx[6];
  ConstTetrahedronEdgeModelPtr tempy[6];
  ConstTetrahedronEdgeModelPtr tempz[6];

  tempx[0] = this->GetConstSelfPtr();
  tempx[1] = reg.GetTetrahedronEdgeModel(x_ModelName1);
  tempx[2] = reg.GetTetrahedronEdgeModel(x_ModelName2);
  tempx[3] = reg.GetTetrahedronEdgeModel(x_ModelName3);
  tempy[0] = reg.GetTetrahedronEdgeModel(y_ModelName0);
  tempy[1] = reg.GetTetrahedronEdgeModel(y_ModelName1);
  tempy[2] = reg.GetTetrahedronEdgeModel(y_ModelName2);
  tempy[3] = reg.GetTetrahedronEdgeModel(y_ModelName3);
  tempz[0] = reg.GetTetrahedronEdgeModel(z_ModelName0);
  tempz[1] = reg.GetTetrahedronEdgeModel(z_ModelName1);
  tempz[2] = reg.GetTetrahedronEdgeModel(z_ModelName2);
  tempz[3] = reg.GetTetrahedronEdgeModel(z_ModelName3);

  for (size_t i = 0; i < 4; ++i)
  {
    dsAssert(tempx[i].get(), "UNEXPECTED");
    dsAssert(tempy[i].get(), "UNEXPECTED");
    dsAssert(tempz[i].get(), "UNEXPECTED");
  }


  const ConstTetrahedronList &tl = GetRegion().GetTetrahedronList();


  std::vector<std::vector<DoubleType> > evx(4);
  std::vector<std::vector<DoubleType> > evy(4);
  std::vector<std::vector<DoubleType> > evz(4);
  for (size_t i = 0; i < 4; ++i)
  {
    evx[i].resize(6*tl.size());
    evy[i].resize(6*tl.size());
    evz[i].resize(6*tl.size());
  }

  const TetrahedronElementField<DoubleType> &efield = reg.GetTetrahedronElementField<DoubleType>();
  typename TetrahedronElementField<DoubleType>::DerivativeEdgeVectors_t v;

  //// For each tetrahedron
  for (size_t i = 0; i < tl.size(); ++i)
  {
    const Tetrahedron &tetrahedron = *tl[i];
    efield.GetTetrahedronElementField(tetrahedron, *emp[0], *emp[1], v);
    for (size_t nindex = 0; nindex < 4; ++nindex)
    {
      for (size_t eindex = 0; eindex < 6; ++eindex)
      {
        const size_t oindex = 6*i + eindex;
        const Vector<DoubleType> &vec = v[nindex][eindex];
        evx[nindex][oindex] = vec.Getx();
        evy[nindex][oindex] = vec.Gety();
        evz[nindex][oindex] = vec.Getz();
      }
    }
  }
  for (size_t i = 0; i < 4; ++i)
  {
    std::const_pointer_cast<TetrahedronEdgeModel, const TetrahedronEdgeModel>(tempx[i])->SetValues(evx[i]);
    std::const_pointer_cast<TetrahedronEdgeModel, const TetrahedronEdgeModel>(tempy[i])->SetValues(evy[i]);
    std::const_pointer_cast<TetrahedronEdgeModel, const TetrahedronEdgeModel>(tempz[i])->SetValues(evz[i]);
  }
}

template <typename DoubleType>
void TetrahedronEdgeFromEdgeModelDerivative<DoubleType>::Serialize(std::ostream &of) const
{
  of << "COMMAND element_pair_from_edge_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -edge_model \"" << edgeModelName << "\" -derivative \"" << nodeModelName << "\"";
}

template class TetrahedronEdgeFromEdgeModelDerivative<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class TetrahedronEdgeFromEdgeModelDerivative<float128>;
#endif

TetrahedronEdgeModelPtr CreateTetrahedronEdgeFromEdgeModelDerivative(
        const std::string &edgemodel,
        const std::string &derivative,
        RegionPtr rp
    )
{
  const bool use_extended = rp->UseExtendedPrecisionModels();
  return create_tetrahedron_edge_model<TetrahedronEdgeFromEdgeModelDerivative<double>, TetrahedronEdgeFromEdgeModelDerivative<extended_type>>(use_extended, edgemodel, derivative, rp);
}

