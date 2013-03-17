/***
DEVSIM
Copyright 2013 Devsim LLC

This file is part of DEVSIM.

DEVSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, version 3.

DEVSIM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with DEVSIM.  If not, see <http://www.gnu.org/licenses/>.
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

