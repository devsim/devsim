/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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


template <typename DoubleType>
EdgeSubModel<DoubleType>::EdgeSubModel(const std::string &nm, RegionPtr rp, EdgeModel::DisplayType dt)
    :
        EdgeModel(nm, rp, dt)
{
}

template <typename DoubleType>
EdgeSubModel<DoubleType>::EdgeSubModel(const std::string &nm, RegionPtr rp, EdgeModel::DisplayType dt, ConstEdgeModelPtr nmp)
    :
        EdgeModel(nm, rp, dt),
        parentModel(nmp)
{
}

template <typename DoubleType>
void EdgeSubModel<DoubleType>::derived_init()
{
  if (auto p = parentModel.lock())
  {
    parentModelName = p->GetName();
    RegisterCallback(parentModelName);
  }
}

template <typename DoubleType>
EdgeModelPtr EdgeSubModel<DoubleType>::CreateEdgeSubModel(const std::string &nm, RegionPtr rp, EdgeModel::DisplayType dt)
{
  auto p = dsModelFactory<EdgeSubModel>::create(nm, rp, dt);
  return p;
}

template <typename DoubleType>
EdgeModelPtr EdgeSubModel<DoubleType>::CreateEdgeSubModel(const std::string &nm, RegionPtr rp, EdgeModel::DisplayType dt, ConstEdgeModelPtr nmp)
{
  auto p = dsModelFactory<EdgeSubModel>::create(nm, rp, dt, nmp);
  return p;
}

template <typename DoubleType>
void EdgeSubModel<DoubleType>::calcEdgeScalarValues() const
{
    if (!parentModelName.empty())
    {
      ConstEdgeModelPtr emp = GetRegion().GetEdgeModel(parentModelName);
      if (!parentModel.expired())
      {
        parentModel.lock()->template GetScalarValues<DoubleType>();
      }
      else if (emp)
      {
        parentModel.reset();
        dsErrors::ChangedModelModelDependency(GetRegion(), parentModelName, dsErrors::ModelInfo::EDGE, GetName(), dsErrors::ModelInfo::EDGE, OutputStream::OutputType::INFO);
        parentModelName.clear();
      }
      else
      {
        dsErrors::MissingModelModelDependency(GetRegion(), parentModelName, dsErrors::ModelInfo::EDGE, GetName(), dsErrors::ModelInfo::EDGE, OutputStream::OutputType::FATAL);
      }
    }
}

template <typename DoubleType>
void EdgeSubModel<DoubleType>::Serialize(std::ostream &of) const
{
  if (!parentModelName.empty())
  {
    of << "DATAPARENT \"" << parentModelName << "\"";
  }
  else if (this->IsUniform())
  {
    of << "UNIFORM " << GetUniformValue<DoubleType>();
  }
  else
  {
    of << "DATA";
    const EdgeScalarList<DoubleType> &vals = this->GetScalarValues<DoubleType>();
    for (size_t i = 0; i < vals.size(); ++i)
    {
      of << "\n" << vals[i];
    }
  }
}

template class EdgeSubModel<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class EdgeSubModel<float128>;
#endif

EdgeModelPtr CreateEdgeSubModel(const std::string &nm, RegionPtr rp, EdgeModel::DisplayType dt)
{
  return create_edge_model<EdgeSubModel<double>, EdgeSubModel<extended_type>>(rp->UseExtendedPrecisionModels(), nm, rp, dt);
}

EdgeModelPtr CreateEdgeSubModel(const std::string &nm, RegionPtr rp, EdgeModel::DisplayType dt, EdgeModelPtr emp)
{
  return create_edge_model<EdgeSubModel<double>, EdgeSubModel<extended_type>>(rp->UseExtendedPrecisionModels(), nm, rp, dt, emp);
}

