/***
DEVSIM
Copyright 2020 Devsim LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
***/

#include "TetrahedronEdgePairFromEdgeModel.hh"
#include "Region.hh"
#include "TetrahedronEdgeSubModel.hh"
#include "TetrahedronElementField.hh"
#include "Vector.hh"
#include "dsAssert.hh"

template <typename DoubleType>
TetrahedronEdgePairFromEdgeModel<DoubleType>::TetrahedronEdgePairFromEdgeModel(
    const std::string &edgemodel, RegionPtr rp)
    : TetrahedronEdgeModel(edgemodel + "_node0_x", rp,
                           TetrahedronEdgeModel::DisplayType::SCALAR),
      edgeModelName(edgemodel) {
  RegisterCallback(edgemodel);

  model_names = {{
      {edgemodel + "_node0_x", edgemodel + "_node0_y", edgemodel + "_node0_z"},
      {edgemodel + "_node1_x", edgemodel + "_node1_y", edgemodel + "_node1_z"},
  }};

  new TetrahedronEdgeSubModel<DoubleType>(
      model_names[0][1], rp, TetrahedronEdgeModel::DisplayType::SCALAR,
      this->GetSelfPtr());
  new TetrahedronEdgeSubModel<DoubleType>(
      model_names[0][2], rp, TetrahedronEdgeModel::DisplayType::SCALAR,
      this->GetSelfPtr());
  new TetrahedronEdgeSubModel<DoubleType>(
      model_names[1][0], rp, TetrahedronEdgeModel::DisplayType::SCALAR,
      this->GetSelfPtr());
  new TetrahedronEdgeSubModel<DoubleType>(
      model_names[1][1], rp, TetrahedronEdgeModel::DisplayType::SCALAR,
      this->GetSelfPtr());
  new TetrahedronEdgeSubModel<DoubleType>(
      model_names[1][2], rp, TetrahedronEdgeModel::DisplayType::SCALAR,
      this->GetSelfPtr());
}

template <typename DoubleType>
void TetrahedronEdgePairFromEdgeModel<
    DoubleType>::calcTetrahedronEdgeScalarValues() const {
  const Region &reg = GetRegion();

  const ConstEdgeModelPtr emp = reg.GetEdgeModel(edgeModelName);
  dsAssert(emp.get(), "UNEXPECTED");

  const ConstTetrahedronList &tl = GetRegion().GetTetrahedronList();

  std::array<std::array<TetrahedronEdgeModelPtr, 3>, 2> temp;
  std::array<std::array<std::vector<DoubleType>, 3>, 2> ev;
  for (size_t i = 0; i < 2; ++i) {
    for (size_t j = 0; j < 3; ++j) {
      ConstTetrahedronEdgeModelPtr t =
          reg.GetTetrahedronEdgeModel(model_names[i][j]);
      dsAssert(t.get(), "UNEXPECTED");
      temp[i][j] = std::const_pointer_cast<TetrahedronEdgeModel,
                                           const TetrahedronEdgeModel>(t);
      ev[i][j].resize(6 * tl.size());
    }
  }

  const TetrahedronElementField<DoubleType> &efield =
      reg.GetTetrahedronElementField<DoubleType>();
  std::array<typename TetrahedronElementField<DoubleType>::EdgeVectors_t, 2> v;
  for (size_t i = 0; i < tl.size(); ++i) {
    const Tetrahedron &tetrahedron = *tl[i];
    efield.GetTetrahedronElementFieldPairs(tetrahedron, *emp, v[0], v[1]);
    for (size_t j = 0; j < 6; ++j) {
      for (size_t k = 0; k < 2; ++k) {
        const Vector<DoubleType> &vec = v[k][j];
        ev[k][0][6 * i + j] = vec.Getx();
        ev[k][1][6 * i + j] = vec.Gety();
        ev[k][2][6 * i + j] = vec.Getz();
      }
    }
  }

  for (size_t i = 0; i < 2; ++i) {
    for (size_t j = 0; j < 3; ++j) {
      temp[i][j]->SetValues(ev[i][j]);
    }
  }
}

template <typename DoubleType>
void TetrahedronEdgePairFromEdgeModel<DoubleType>::Serialize(
    std::ostream &of) const {
  of << "COMMAND element_pair_from_edge_model -device \"" << GetDeviceName()
     << "\" -region \"" << GetRegionName() << "\" -edge_model \""
     << edgeModelName << "\"";
}

template class TetrahedronEdgePairFromEdgeModel<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class TetrahedronEdgePairFromEdgeModel<float128>;
#endif

TetrahedronEdgeModelPtr
CreateTetrahedronEdgePairFromEdgeModel(const std::string &edgemodel,
                                       RegionPtr rp) {
  const bool use_extended = rp->UseExtendedPrecisionModels();
  return create_tetrahedron_edge_model<
      TetrahedronEdgePairFromEdgeModel<double>,
      TetrahedronEdgePairFromEdgeModel<extended_type>>(use_extended, edgemodel,
                                                       rp);
}
