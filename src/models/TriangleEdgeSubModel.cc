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

#include "TriangleEdgeSubModel.hh"
#include "Device.hh"
#include "Edge.hh"
#include "GeometryStream.hh"
#include "ModelErrors.hh"
#include "Region.hh"
#include "Vector.hh"
#include "dsAssert.hh"

#include <sstream>

template <typename DoubleType>
TriangleEdgeSubModel<DoubleType>::TriangleEdgeSubModel(
    const std::string &nm, RegionPtr rp, TriangleEdgeModel::DisplayType dt)
    : TriangleEdgeModel(nm, rp, dt), parentModel() {}

template <typename DoubleType>
TriangleEdgeSubModel<DoubleType>::TriangleEdgeSubModel(
    const std::string &nm, RegionPtr rp, TriangleEdgeModel::DisplayType dt,
    ConstTriangleEdgeModelPtr nmp)
    : TriangleEdgeModel(nm, rp, dt), parentModel(nmp) {
  parentModelName = parentModel.lock()->GetName();

  //// TODO: consider making it so that we have different kinds of callbacks
  RegisterCallback(parentModelName);
#if 0
    os << "creating TriangleEdgeSubModel " << nm << " with parent " << parentModel->GetName() << "\n";
#endif
}

template <typename DoubleType>
TriangleEdgeModelPtr
TriangleEdgeSubModel<DoubleType>::CreateTriangleEdgeSubModel(
    const std::string &nm, RegionPtr rp, TriangleEdgeModel::DisplayType dt) {
  TriangleEdgeModel *p = new TriangleEdgeSubModel(nm, rp, dt);
  return p->GetSelfPtr();
}

template <typename DoubleType>
TriangleEdgeModelPtr
TriangleEdgeSubModel<DoubleType>::CreateTriangleEdgeSubModel(
    const std::string &nm, RegionPtr rp, TriangleEdgeModel::DisplayType dt,
    ConstTriangleEdgeModelPtr nmp) {
  TriangleEdgeModel *p = new TriangleEdgeSubModel(nm, rp, dt, nmp);
  return p->GetSelfPtr();
}

template <typename DoubleType>
void TriangleEdgeSubModel<DoubleType>::calcTriangleEdgeScalarValues() const {
  if (!parentModelName.empty()) {
    ConstTriangleEdgeModelPtr emp =
        GetRegion().GetTriangleEdgeModel(parentModelName);
    if (!parentModel.expired()) {
      parentModel.lock()->template GetScalarValues<DoubleType>();
    } else if (emp != parentModel.lock()) {
      parentModel.reset();
      dsErrors::ChangedModelModelDependency(
          GetRegion(), parentModelName, dsErrors::ModelInfo::ELEMENTEDGE,
          GetName(), dsErrors::ModelInfo::ELEMENTEDGE,
          OutputStream::OutputType::INFO);
      parentModelName.clear();
    } else {
      dsAssert(0, "UNEXPECTED");
    }
  }
}

template <typename DoubleType>
void TriangleEdgeSubModel<DoubleType>::Serialize(std::ostream &of) const {
  if (!parentModelName.empty()) {
    of << "DATAPARENT \"" << parentModelName << "\"";
  } else if (this->IsUniform()) {
    of << "UNIFORM " << GetUniformValue<DoubleType>();
  } else {
    of << "DATA\n";
    const TriangleEdgeScalarList<DoubleType> &vals =
        this->GetScalarValues<DoubleType>();
    for (size_t i = 0; i < vals.size(); ++i) {
      of << vals[i] << "\n";
    }
  }
}

template class TriangleEdgeSubModel<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class TriangleEdgeSubModel<float128>;
#endif

TriangleEdgeModelPtr
CreateTriangleEdgeSubModel(const std::string &nm, RegionPtr rp,
                           TriangleEdgeModel::DisplayType dt) {
  return create_triangle_edge_model<TriangleEdgeSubModel<double>,
                                    TriangleEdgeSubModel<extended_type>>(
      rp->UseExtendedPrecisionModels(), nm, rp, dt);
}

TriangleEdgeModelPtr
CreateTriangleEdgeSubModel(const std::string &nm, RegionPtr rp,
                           TriangleEdgeModel::DisplayType dt,
                           TriangleEdgeModelPtr emp) {
  return create_triangle_edge_model<TriangleEdgeSubModel<double>,
                                    TriangleEdgeSubModel<extended_type>>(
      rp->UseExtendedPrecisionModels(), nm, rp, dt, emp);
}
