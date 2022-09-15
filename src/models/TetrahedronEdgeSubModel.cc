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

#include "TetrahedronEdgeSubModel.hh"
#include "Device.hh"
#include "Edge.hh"
#include "GeometryStream.hh"
#include "ModelErrors.hh"
#include "Region.hh"
#include "Vector.hh"
#include "dsAssert.hh"

#include <sstream>

template <typename DoubleType>
TetrahedronEdgeSubModel<DoubleType>::TetrahedronEdgeSubModel(
    const std::string &nm, RegionPtr rp, TetrahedronEdgeModel::DisplayType dt)
    : TetrahedronEdgeModel(nm, rp, dt) {}

template <typename DoubleType>
TetrahedronEdgeSubModel<DoubleType>::TetrahedronEdgeSubModel(
    const std::string &nm, RegionPtr rp, TetrahedronEdgeModel::DisplayType dt,
    ConstTetrahedronEdgeModelPtr nmp)
    : TetrahedronEdgeModel(nm, rp, dt), parentModel(nmp) {
  parentModelName = parentModel.lock()->GetName();

  RegisterCallback(parentModelName);
}

template <typename DoubleType>
TetrahedronEdgeModelPtr
TetrahedronEdgeSubModel<DoubleType>::CreateTetrahedronEdgeSubModel(
    const std::string &nm, RegionPtr rp, TetrahedronEdgeModel::DisplayType dt) {
  TetrahedronEdgeModel *p = new TetrahedronEdgeSubModel(nm, rp, dt);
  return p->GetSelfPtr();
}

template <typename DoubleType>
TetrahedronEdgeModelPtr
TetrahedronEdgeSubModel<DoubleType>::CreateTetrahedronEdgeSubModel(
    const std::string &nm, RegionPtr rp, TetrahedronEdgeModel::DisplayType dt,
    ConstTetrahedronEdgeModelPtr nmp) {
  TetrahedronEdgeModel *p = new TetrahedronEdgeSubModel(nm, rp, dt, nmp);
  return p->GetSelfPtr();
}

template <typename DoubleType>
void TetrahedronEdgeSubModel<DoubleType>::calcTetrahedronEdgeScalarValues()
    const {
  if (!parentModelName.empty()) {
#if 0
      os << "updating TetrahedronEdgeSubModel " << GetName() << " from parent " << parentModel->GetName() << "\n";
#endif
    ConstTetrahedronEdgeModelPtr emp =
        GetRegion().GetTetrahedronEdgeModel(parentModelName);
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
void TetrahedronEdgeSubModel<DoubleType>::Serialize(std::ostream &of) const {
  if (!parentModelName.empty()) {
    of << "DATAPARENT \"" << parentModelName << "\"";
  } else if (this->IsUniform()) {
    of << "UNIFORM " << GetUniformValue<DoubleType>();
  } else {
    of << "DATA";
    const TetrahedronEdgeScalarList<DoubleType> &vals =
        this->GetScalarValues<DoubleType>();
    for (size_t i = 0; i < vals.size(); ++i) {
      of << "\n" << vals[i];
    }
  }
}

template class TetrahedronEdgeSubModel<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class TetrahedronEdgeSubModel<float128>;
#endif

TetrahedronEdgeModelPtr
CreateTetrahedronEdgeSubModel(const std::string &nm, RegionPtr rp,
                              TetrahedronEdgeModel::DisplayType dt) {
  return create_tetrahedron_edge_model<TetrahedronEdgeSubModel<double>,
                                       TetrahedronEdgeSubModel<extended_type>>(
      rp->UseExtendedPrecisionModels(), nm, rp, dt);
}

TetrahedronEdgeModelPtr
CreateTetrahedronEdgeSubModel(const std::string &nm, RegionPtr rp,
                              TetrahedronEdgeModel::DisplayType dt,
                              TetrahedronEdgeModelPtr emp) {
  return create_tetrahedron_edge_model<TetrahedronEdgeSubModel<double>,
                                       TetrahedronEdgeSubModel<extended_type>>(
      rp->UseExtendedPrecisionModels(), nm, rp, dt, emp);
}
