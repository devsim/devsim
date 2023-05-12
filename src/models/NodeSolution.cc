/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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

template <typename DoubleType>
NodeSolution<DoubleType>::NodeSolution(const std::string &nm, RegionPtr rp, NodeModel::DisplayType dt)
    :
        NodeModel(nm, rp, dt),
        parentModel()
{
}

template <typename DoubleType>
NodeSolution<DoubleType>::NodeSolution(const std::string &nm, RegionPtr rp, NodeModel::DisplayType dt, NodeModelPtr nmp)
    :
        NodeModel(nm, rp, dt),
        parentModel(nmp)
{
  parentModelName = parentModel.lock()->GetName();
  RegisterCallback(parentModelName);
}

template <typename DoubleType>
void NodeSolution<DoubleType>::calcNodeScalarValues() const
{
    if (!parentModelName.empty())
    {
        ConstNodeModelPtr nmp = GetRegion().GetNodeModel(parentModelName);
        if (!parentModel.expired())
        {
          parentModel.lock()->template GetScalarValues<DoubleType>();
        }
        else if (nmp)
        {
          parentModel.reset();
          dsErrors::ChangedModelModelDependency(GetRegion(), parentModelName, dsErrors::ModelInfo::NODE, GetName(), dsErrors::ModelInfo::NODE, OutputStream::OutputType::INFO);
          parentModelName.clear();
        }
        else
        {
          dsErrors::MissingModelModelDependency(GetRegion(), parentModelName, dsErrors::ModelInfo::NODE, GetName(), dsErrors::ModelInfo::NODE, OutputStream::OutputType::FATAL);
        }
    }
}

template <typename DoubleType>
void NodeSolution<DoubleType>::setInitialValues()
{
    DefaultInitializeValues();
}

template <typename DoubleType>
void NodeSolution<DoubleType>::Serialize(std::ostream &of) const
{
  if (!parentModelName.empty())
  {
    of << "DATAPARENT \"" << parentModelName << "\"";
  }
  else if (this->IsUniform())
  {
    of << "UNIFORM " << GetUniformValue<DoubleType>() << "";
  }
  else
  {
    of << "DATA";
    const NodeScalarList<DoubleType> &vals = this->GetScalarValues<DoubleType>();
    for (size_t i = 0; i < vals.size(); ++i)
    {
      of << "\n" << vals[i];
    }
  }
}

template class NodeSolution<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class NodeSolution<float128>;
#endif

NodeModelPtr CreateNodeSolution(const std::string &nm, RegionPtr rp, NodeModel::DisplayType dt)
{
  return create_node_model<NodeSolution<double>, NodeSolution<extended_type>>(rp->UseExtendedPrecisionModels(), nm, rp, dt);
}

NodeModelPtr CreateNodeSolution(const std::string &nm, RegionPtr rp, NodeModel::DisplayType dt, NodeModelPtr nmp)
{
  return create_node_model<NodeSolution<double>, NodeSolution<extended_type>>(rp->UseExtendedPrecisionModels(), nm, rp, dt, nmp);
}


