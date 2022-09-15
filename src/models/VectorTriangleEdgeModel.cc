/***
DEVSIM
Copyright 2013 Devsim LLC

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

#include "VectorTriangleEdgeModel.hh"
#include "Region.hh"
#include "TriangleEdgeSubModel.hh"
#include "TriangleElementField.hh"
#include "Vector.hh"
#include "dsAssert.hh"

template <typename DoubleType>
VectorTriangleEdgeModel<DoubleType>::VectorTriangleEdgeModel(
    const std::string &edgemodel, RegionPtr rp)
    : TriangleEdgeModel(edgemodel + "_x", rp,
                        TriangleEdgeModel::DisplayType::SCALAR),
      elementEdgeModelName(edgemodel),
      y_ModelName(elementEdgeModelName + "_y") {
  RegisterCallback(elementEdgeModelName);
  new TriangleEdgeSubModel<DoubleType>(y_ModelName, rp,
                                       TriangleEdgeModel::DisplayType::SCALAR,
                                       this->GetSelfPtr());
}

template <typename DoubleType>
void VectorTriangleEdgeModel<DoubleType>::calcTriangleEdgeScalarValues() const {
  const Region &reg = GetRegion();

  const ConstTriangleEdgeModelPtr emp =
      reg.GetTriangleEdgeModel(elementEdgeModelName);
  dsAssert(emp.get(), "UNEXPECTED");

  const ConstTriangleEdgeModelPtr tempy = reg.GetTriangleEdgeModel(y_ModelName);
  dsAssert(tempy.get(), "UNEXPECTED");

  const ConstTriangleEdgeModelPtr eec =
      reg.GetTriangleEdgeModel("ElementEdgeCouple");
  dsAssert(eec.get(), "UNEXPECTED");

  const ConstTriangleList &tl = GetRegion().GetTriangleList();

  std::vector<DoubleType> evx(3 * tl.size());
  std::vector<DoubleType> evy(3 * tl.size());

  const TriangleElementField<DoubleType> &efield =
      reg.GetTriangleElementField<DoubleType>();
  typename TriangleElementField<DoubleType>::EdgeVectors_t v;
  for (size_t i = 0; i < tl.size(); ++i) {
    const Triangle &triangle = *tl[i];
    efield.GetTriangleElementField(triangle, *eec, *emp, v);
    for (size_t j = 0; j < 3; ++j) {
      evx[3 * i + j] = v[j].Getx();
      evy[3 * i + j] = v[j].Gety();
    }
  }

  SetValues(evx);
  std::const_pointer_cast<TriangleEdgeModel, const TriangleEdgeModel>(tempy)
      ->SetValues(evy);
}

template <typename DoubleType>
void VectorTriangleEdgeModel<DoubleType>::Serialize(std::ostream &of) const {
  of << "COMMAND vector_element_model -device \"" << GetDeviceName()
     << "\" -region \"" << GetRegionName() << "\" -element_model \""
     << elementEdgeModelName << "\"";
}

template class VectorTriangleEdgeModel<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class VectorTriangleEdgeModel<float128>;
#endif

TriangleEdgeModelPtr CreateVectorTriangleEdgeModel(const std::string &edgemodel,
                                                   RegionPtr rp) {
  const bool use_extended = rp->UseExtendedPrecisionModels();
  return create_triangle_edge_model<VectorTriangleEdgeModel<double>,
                                    VectorTriangleEdgeModel<extended_type>>(
      use_extended, edgemodel, rp);
}
