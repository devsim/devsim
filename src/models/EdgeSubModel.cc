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

#include "EdgeSubModel.hh"
#include "Region.hh"
#include "Edge.hh"
#include "Vector.hh"
#include "Device.hh"
#include "dsAssert.hh"
#include "GeometryStream.hh"
#include "ModelErrors.hh"


#include <sstream>


EdgeSubModel::EdgeSubModel(const std::string &nm, RegionPtr rp, EdgeModel::DisplayType dt)
    :
        EdgeModel(nm, rp, dt)
{
}

EdgeSubModel::EdgeSubModel(const std::string &nm, RegionPtr rp, EdgeModel::DisplayType dt, ConstEdgeModelPtr nmp)
    :
        EdgeModel(nm, rp, dt),
        parentModel(nmp)
{
    parentModelName = parentModel.lock()->GetName();

    //// TODO: consider making it so that we have different kinds of callbacks
    RegisterCallback(parentModelName);
#if 0
    os << "creating EdgeSubModel " << nm << " with parent " << parentModel->GetName() << "\n";
#endif
}

EdgeModelPtr EdgeSubModel::CreateEdgeSubModel(const std::string &nm, RegionPtr rp, EdgeModel::DisplayType dt)
{
  EdgeModel *p = new EdgeSubModel(nm, rp, dt);
  return p->GetSelfPtr();
}

EdgeModelPtr EdgeSubModel::CreateEdgeSubModel(const std::string &nm, RegionPtr rp, EdgeModel::DisplayType dt, ConstEdgeModelPtr nmp)
{
  EdgeModel *p = new EdgeSubModel(nm, rp, dt, nmp);
  return p->GetSelfPtr();
}

void EdgeSubModel::calcEdgeScalarValues() const
{
    if (!parentModelName.empty())
    {
#if 0
      os << "updating EdgeSubModel " << GetName() << " from parent " << parentModel->GetName() << "\n";
#endif
      ConstEdgeModelPtr emp = GetRegion().GetEdgeModel(parentModelName);
      if (!parentModel.expired())
      {
        parentModel.lock()->GetScalarValues();
      }
      else if (emp)
      {
        parentModel.reset();
        dsErrors::ChangedModelModelDependency(GetRegion(), parentModelName, dsErrors::ModelInfo::EDGE, GetName(), dsErrors::ModelInfo::EDGE, OutputStream::INFO);
        parentModelName.clear();
      }
      else
      {
        dsErrors::MissingModelModelDependency(GetRegion(), parentModelName, dsErrors::ModelInfo::EDGE, GetName(), dsErrors::ModelInfo::EDGE, OutputStream::FATAL);
      }
    }
}

void EdgeSubModel::Serialize(std::ostream &of) const
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
    const EdgeScalarList &vals = this->GetScalarValues();
    for (size_t i = 0; i < vals.size(); ++i)
    {
      of << "\n" << vals[i];
    }
  }
}

