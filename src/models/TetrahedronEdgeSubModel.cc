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
#include "Region.hh"
#include "Edge.hh"
#include "Vector.hh"
#include "Device.hh"
#include "dsAssert.hh"
#include "GeometryStream.hh"
#include "ModelErrors.hh"

#include <sstream>



TetrahedronEdgeSubModel::TetrahedronEdgeSubModel(const std::string &nm, RegionPtr rp, TetrahedronEdgeModel::DisplayType dt)
    :
        TetrahedronEdgeModel(nm, rp, dt)
{
}

TetrahedronEdgeSubModel::TetrahedronEdgeSubModel(const std::string &nm, RegionPtr rp, ConstTetrahedronEdgeModelPtr nmp, TetrahedronEdgeModel::DisplayType dt)
    :
        TetrahedronEdgeModel(nm, rp, dt),
        parentModel(nmp)
{
    parentModelName = parentModel.lock()->GetName();

    //// TODO: consider making it so that we have different kinds of callbacks
    RegisterCallback(parentModelName);
#if 0
    os << "creating TetrahedronEdgeSubModel " << nm << " with parent " << parentModel->GetName() << "\n";
#endif
}

TetrahedronEdgeModelPtr TetrahedronEdgeSubModel::CreateTetrahedronEdgeSubModel(const std::string &nm, RegionPtr rp, TetrahedronEdgeModel::DisplayType dt)
{
  TetrahedronEdgeModel *p = new TetrahedronEdgeSubModel(nm, rp, dt);
  return p->GetSelfPtr();
}

TetrahedronEdgeModelPtr TetrahedronEdgeSubModel::CreateTetrahedronEdgeSubModel(const std::string &nm, RegionPtr rp, TetrahedronEdgeModel::DisplayType dt, ConstTetrahedronEdgeModelPtr nmp)
{
  TetrahedronEdgeModel *p = new TetrahedronEdgeSubModel(nm, rp, nmp, dt);
  return p->GetSelfPtr();
}


void TetrahedronEdgeSubModel::calcTetrahedronEdgeScalarValues() const
{
    if (!parentModelName.empty())
    {
#if 0
      os << "updating TetrahedronEdgeSubModel " << GetName() << " from parent " << parentModel->GetName() << "\n";
#endif
      ConstTetrahedronEdgeModelPtr emp = GetRegion().GetTetrahedronEdgeModel(parentModelName);
      if (!parentModel.expired())
      {
        parentModel.lock()->GetScalarValues();
      }
      else if (emp != parentModel.lock())
      {
        parentModel.reset();
        dsErrors::ChangedModelModelDependency(GetRegion(), parentModelName, dsErrors::ModelInfo::ELEMENTEDGE, GetName(), dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::INFO);
        parentModelName.clear();
      }
      else
      {
        dsAssert(0, "UNEXPECTED");
      }
    }
}

void TetrahedronEdgeSubModel::Serialize(std::ostream &of) const
{
  if (!parentModelName.empty())
  {
    of << "DATAPARENT \"" << parentModelName << "\"";
  }
  else if (this->IsUniform())
  {
    of << "UNIFORM " << GetUniformValue();
  }
  else
  {
    of << "DATA";
    const TetrahedronEdgeScalarList &vals = this->GetScalarValues();
    for (size_t i = 0; i < vals.size(); ++i)
    {
      of << "\n" << vals[i];
    }
  }
}

