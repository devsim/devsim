/***
DEVSIM
Copyright 2020 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "TriangleEdgePairFromEdgeModel.hh"
#include "TriangleEdgeSubModel.hh"
#include "Region.hh"
#include "TriangleElementField.hh"
#include "dsAssert.hh"
#include "Vector.hh"


template <typename DoubleType>
TriangleEdgePairFromEdgeModel<DoubleType>::TriangleEdgePairFromEdgeModel(const std::string &edgemodel, RegionPtr rp)
    : TriangleEdgeModel(edgemodel + "_node0_x", rp, TriangleEdgeModel::DisplayType::SCALAR), edgeModelName(edgemodel)
{
  RegisterCallback(edgemodel);

  model_names = {{
                  {edgemodel + "_node0_x", edgemodel + "_node0_y"},
                  {edgemodel + "_node1_x", edgemodel + "_node1_y"}
                }};
  new TriangleEdgeSubModel<DoubleType>(model_names[0][1], rp, TriangleEdgeModel::DisplayType::SCALAR, this->GetSelfPtr());
  new TriangleEdgeSubModel<DoubleType>(model_names[1][0], rp, TriangleEdgeModel::DisplayType::SCALAR, this->GetSelfPtr());
  new TriangleEdgeSubModel<DoubleType>(model_names[1][1], rp, TriangleEdgeModel::DisplayType::SCALAR, this->GetSelfPtr());
}

template <typename DoubleType>
void TriangleEdgePairFromEdgeModel<DoubleType>::calcTriangleEdgeScalarValues() const
{
  const Region &reg = GetRegion();

  const ConstEdgeModelPtr emp = reg.GetEdgeModel(edgeModelName);
  dsAssert(emp.get(), "UNEXPECTED");

  const ConstTriangleEdgeModelPtr eec = reg.GetTriangleEdgeModel("ElementEdgeCouple");
  dsAssert(eec.get(), "UNEXPECTED");



  const ConstTriangleList &tl = GetRegion().GetTriangleList();

  std::vector<std::vector<DoubleType> > evx(2);
  std::vector<std::vector<DoubleType> > evy(2);
  for (size_t i = 0; i < 2; ++i)
  {
    evx[i].resize(3*tl.size());
    evy[i].resize(3*tl.size());
  }

  const TriangleElementField<DoubleType> &efield = reg.GetTriangleElementField<DoubleType>();
  std::array<typename TriangleElementField<DoubleType>::EdgeVectors_t, 2> v;
  for (size_t i = 0; i < tl.size(); ++i)
  {
    const Triangle &triangle = *tl[i];
    efield.GetTriangleElementFieldPairs(triangle, *eec, *emp, v[0], v[1]);
    for (size_t j = 0; j < 3; ++j)
    {
      evx[0][3*i + j] = v[0][j].Getx();
      evy[0][3*i + j] = v[0][j].Gety();
    }
    for (size_t j = 0; j < 3; ++j)
    {
      evx[1][3*i + j] = v[1][j].Getx();
      evy[1][3*i + j] = v[1][j].Gety();
    }
  }

  for (size_t i = 0; i < 2; ++i)
  {
    auto fieldx = reg.GetTriangleEdgeModel(model_names[i][0]);
    dsAssert(fieldx.get(), "UNEXPECTED");
    std::const_pointer_cast<TriangleEdgeModel, const TriangleEdgeModel>(fieldx)->SetValues(evx[i]);

    auto fieldy = reg.GetTriangleEdgeModel(model_names[i][1]);
    dsAssert(fieldy.get(), "UNEXPECTED");
    std::const_pointer_cast<TriangleEdgeModel, const TriangleEdgeModel>(fieldy)->SetValues(evy[i]);
  }
}

template <typename DoubleType>
void TriangleEdgePairFromEdgeModel<DoubleType>::Serialize(std::ostream &of) const
{
  of << "COMMAND element_pair_from_edge_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -edge_model \"" << edgeModelName << "\"";
}

template class TriangleEdgePairFromEdgeModel<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class TriangleEdgePairFromEdgeModel<float128>;
#endif


TriangleEdgeModelPtr CreateTriangleEdgePairFromEdgeModel(const std::string &edgemodel, RegionPtr rp)
{
  const bool use_extended = rp->UseExtendedPrecisionModels();
  return create_triangle_edge_model<TriangleEdgePairFromEdgeModel<double>, TriangleEdgePairFromEdgeModel<extended_type>>(use_extended, edgemodel, rp);
}



