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

