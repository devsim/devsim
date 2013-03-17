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

#include "NodeSolution.hh"
#include "Region.hh"
#include "Node.hh"
#include "Vector.hh"
#include "Device.hh"
#include "dsAssert.hh"
#include "GeometryStream.hh"
#include "ModelErrors.hh"


#include <sstream>

NodeSolution::NodeSolution(const std::string &nm, RegionPtr rp)
    :
        NodeModel(nm, rp, NodeModel::SCALAR),
        parentModel()
{
#if 0
    os << "creating NodeSolution " << nm << "\n";
#endif
}

NodeSolution::NodeSolution(const std::string &nm, RegionPtr rp, NodeModelPtr nmp)
    :
        // dirty hack
        NodeModel(nm, rp, NodeModel::SCALAR),
        parentModel(nmp)
{
  parentModelName = parentModel.lock()->GetName();
  RegisterCallback(parentModelName);
#if 0
    os << "creating NodeSolution " << nm << " with parent " << parentModel->GetName() << "\n";
#endif
}

NodeModelPtr NodeSolution::CreateNodeSolution(const std::string &nm, RegionPtr rp)
{
  NodeModel *p = new NodeSolution(nm, rp);
  return p->GetSelfPtr();
}

NodeModelPtr NodeSolution::CreateNodeSolution(const std::string &nm, RegionPtr rp, NodeModelPtr nmp)
{
  NodeModel *p = new NodeSolution(nm, rp, nmp);
  return p->GetSelfPtr();
}

void NodeSolution::calcNodeScalarValues() const
{
    if (!parentModelName.empty())
    {
#if 0
        os << "updating NodeSolution " << GetName() << " from parent " << parentModel->GetName() << "\n";
#endif
        ConstNodeModelPtr nmp = GetRegion().GetNodeModel(parentModelName);
        if (!parentModel.expired())
        {
          parentModel.lock()->GetScalarValues();
        }
        else if (nmp)
        {
          parentModel.reset();
          dsErrors::ChangedModelModelDependency(GetRegion(), parentModelName, dsErrors::ModelInfo::NODE, GetName(), dsErrors::ModelInfo::NODE, OutputStream::INFO);
          parentModelName.clear();
        }
        else
        {
          dsErrors::MissingModelModelDependency(GetRegion(), parentModelName, dsErrors::ModelInfo::NODE, GetName(), dsErrors::ModelInfo::NODE, OutputStream::FATAL);
        }
    }
}

void NodeSolution::setInitialValues()
{
    DefaultInitializeValues();
}

void NodeSolution::Serialize(std::ostream &of) const
{
  if (!parentModelName.empty())
  {
    of << "DATAPARENT \"" << parentModelName << "\"";
  }
  else if (this->IsUniform())
  {
    of << "UNIFORM " << GetUniformValue() << "";
  }
  else
  {
    of << "DATA";
    const NodeScalarList &vals = this->GetScalarValues();
    for (size_t i = 0; i < vals.size(); ++i)
    {
      of << "\n" << vals[i];
    }
  }
}

