/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "AverageEdgeModel.hh"
#include "dsAssert.hh"
#include "Region.hh"
#include "GeometryStream.hh"
#include "ModelErrors.hh"
#include "EdgeSubModel.hh"
#include "NodeModel.hh"
#include "EdgeScalarData.hh"
#include "Edge.hh"
#include "Node.hh"
#include "ModelErrors.hh"

#include <cmath>
#include <sstream>

AverageEdgeModelEnum::TypeNameMap_t AverageEdgeModelEnum::AverageTypeNames[] = {
        {"arithmetic", AverageEdgeModelEnum::ARITHMETIC},
        {"geometric", AverageEdgeModelEnum::GEOMETRIC},
        {"gradient", AverageEdgeModelEnum::GRADIENT},
        {"negative_gradient", AverageEdgeModelEnum::NEGATIVE_GRADIENT},
        {nullptr, AverageEdgeModelEnum::UNKNOWN}};

template <typename DoubleType>
AverageEdgeModel<DoubleType>::~AverageEdgeModel()
{
  // We can't do this, it causes seg fault if the region is already gone
#if 0
  if (!edgeModel1Name.empty())
  {
    dsErrors::ModelModelDeletion(GetRegion(), edgeModel1Name, dsErrors::ModelInfo::EDGE, GetName(), dsErrors::ModelInfo::EDGE, OutputStream::OutputType::INFO);
    ///// This is EVIL
    //const_cast<Region &>(GetRegion()).DeleteEdgeModel(edgeModel1Name);
  }
#endif
}

template <typename DoubleType>
AverageEdgeModel<DoubleType>::AverageEdgeModel(const std::string &emodel, const std::string &nmodel, AverageEdgeModelEnum::AverageType_t atype, RegionPtr rp)
    :
        EdgeModel(emodel, rp, EdgeModel::DisplayType::SCALAR),
        originalEdgeModelName(emodel),
        nodeModelName(nmodel),
        averageType(atype)
{
    dsAssert(rp->GetNodeModel(nmodel).get(), "UNEXPECTED");
    RegisterCallback(nodeModelName);

    if ((averageType == AverageEdgeModelEnum::GRADIENT) || (averageType == AverageEdgeModelEnum::NEGATIVE_GRADIENT))
    {
      RegisterCallback("EdgeInverseLength");
    }
}

template <typename DoubleType>
AverageEdgeModel<DoubleType>::AverageEdgeModel(const std::string &emodel, const std::string &nmodel, const std::string &deriv, AverageEdgeModelEnum::AverageType_t atype, RegionPtr rp)
    :
        EdgeModel(emodel + ":" + deriv + "@n0", rp, EdgeModel::DisplayType::SCALAR),
        originalEdgeModelName(emodel),
        nodeModelName(nmodel),
        edgeModel1Name(emodel + ":" + deriv + "@n1"),
        variableName(deriv),
        averageType(atype)
{
    node1EdgeModel = EdgeSubModel<DoubleType>::CreateEdgeSubModel(edgeModel1Name, rp, EdgeModel::DisplayType::SCALAR, this->GetSelfPtr());
    dsAssert(!node1EdgeModel.expired(), "UNEXPECTED");

    if (!rp->GetNodeModel(nodeModelName))
    {
      dsErrors::MissingModelModelDependency(GetRegion(), nodeModelName, dsErrors::ModelInfo::NODE, GetName(), dsErrors::ModelInfo::EDGE, OutputStream::OutputType::FATAL);
      return;
    }

    RegisterCallback(nodeModelName);

    if (!variableName.empty())
    {
      if (variableName != nodeModelName)
      {
        derivativeModelName = nodeModelName + ":" + variableName;
        if (!rp->GetNodeModel(derivativeModelName))
        {
          dsErrors::MissingModelModelDependency(GetRegion(), derivativeModelName, dsErrors::ModelInfo::NODE, GetName(), dsErrors::ModelInfo::EDGE, OutputStream::OutputType::FATAL);
          return;
        }
        RegisterCallback(derivativeModelName);
      }
    }

    if ((averageType == AverageEdgeModelEnum::GRADIENT) || (averageType == AverageEdgeModelEnum::NEGATIVE_GRADIENT))
    {
      RegisterCallback("EdgeInverseLength");
    }
#if 0
    os << "creating AverageEdgeModelModel " << edgemodel0 << " with parent " << rp->GetNodeModel(nodemodel)->GetName() << "\n";
#endif
}

namespace {
struct Arithmetic {
  template <typename DoubleType>
  DoubleType operator()(DoubleType x, DoubleType y) const
  {
    return 0.5 * (x + y);
  }

  template <typename DoubleType>
  std::pair<DoubleType, DoubleType> operator()(DoubleType /*x*/, DoubleType xd, DoubleType /*y*/, DoubleType yd) const
  {
    return std::make_pair(0.5 * xd, 0.5 * yd);
  }
};

struct Geometric {
  template <typename DoubleType>
  DoubleType operator()(DoubleType x, DoubleType y) const
  {
    return sqrt(x*y);
  }

  //// more efficient would be using the non-derivative model and scaling
  template <typename DoubleType>
  std::pair<DoubleType, DoubleType> operator()(DoubleType x, DoubleType xd, DoubleType y, DoubleType yd) const
  {
    DoubleType ratio = sqrt(y/x);
    return std::make_pair(0.5 * xd * ratio, 0.5 * yd /ratio);
  }
};

};

template <typename DoubleType>
void AverageEdgeModel<DoubleType>::calcEdgeScalarValues() const
{
    Region *rp = const_cast<Region *>(&this->GetRegion());

    ConstNodeModelPtr nm = rp->GetNodeModel(nodeModelName);

    ConstNodeModelPtr nmDerivative;
    if (!derivativeModelName.empty())
    {
      nmDerivative = rp->GetNodeModel(derivativeModelName);
      if (!nmDerivative)
      {
        dsErrors::MissingModelModelDependency(*rp, nodeModelName, dsErrors::ModelInfo::NODE, GetName(), dsErrors::ModelInfo::EDGE, OutputStream::OutputType::FATAL);
        return;
      }
    }

    if (!nm)
    {
      dsErrors::MissingModelModelDependency(*rp, nodeModelName, dsErrors::ModelInfo::NODE, GetName(), dsErrors::ModelInfo::EDGE, OutputStream::OutputType::FATAL);
      return;
    }

    if (variableName.empty())
    {
      EdgeScalarList<DoubleType> esl;
      switch (averageType) {
          case AverageEdgeModelEnum::ARITHMETIC:
            doMath(nm, esl, Arithmetic());
            break;
          case AverageEdgeModelEnum::GEOMETRIC:
            doMath(nm, esl, Geometric());
            break;
          case AverageEdgeModelEnum::GRADIENT:
            doGradient(nm, esl, 1.0);
            break;
          case AverageEdgeModelEnum::NEGATIVE_GRADIENT:
            doGradient(nm, esl, -1.0);
            break;
          default:
            break;
      }
      this->SetValues(esl);
    }
    else
    {
      const ConstEdgeModelPtr em1 = rp->GetEdgeModel(edgeModel1Name);
      if (!em1)
      {
        dsErrors::ReviveContainer(*rp, edgeModel1Name, dsErrors::ModelInfo::EDGE, GetName(), dsErrors::ModelInfo::EDGE, OutputStream::OutputType::INFO);
        node1EdgeModel = EdgeSubModel<DoubleType>::CreateEdgeSubModel(edgeModel1Name, rp, EdgeModel::DisplayType::SCALAR, this->GetConstSelfPtr());
      }
      else if (node1EdgeModel.expired())
      {
        dsErrors::ChangedModelModelDependency(*rp, nodeModelName, dsErrors::ModelInfo::NODE, GetName(),      dsErrors::ModelInfo::EDGE, OutputStream::OutputType::ERROR);
        dsErrors::ChangedModelModelDependency(*rp, nodeModelName, dsErrors::ModelInfo::NODE, edgeModel1Name, dsErrors::ModelInfo::EDGE, OutputStream::OutputType::FATAL);
        return;
      }


      EdgeScalarList<DoubleType> esl0;
      EdgeScalarList<DoubleType> esl1;

      switch (averageType) {
          case AverageEdgeModelEnum::ARITHMETIC:
            doMath(nm, nmDerivative, esl0, esl1, Arithmetic());
            break;
          case AverageEdgeModelEnum::GEOMETRIC:
            doMath(nm, nmDerivative, esl0, esl1, Geometric());
            break;
          case AverageEdgeModelEnum::GRADIENT:
            doGradient(nm, nmDerivative, esl0, esl1, 1.0);
            break;
          case AverageEdgeModelEnum::NEGATIVE_GRADIENT:
            doGradient(nm, nmDerivative, esl0, esl1, -1.0);
            break;
          default:
            break;
      }
      this->SetValues(esl0);
      std::const_pointer_cast<EdgeModel, const EdgeModel>(node1EdgeModel.lock())->SetValues(esl1);
    }
}

template <typename DoubleType> template <typename T>
void AverageEdgeModel<DoubleType>::doMath(ConstNodeModelPtr nmp, EdgeScalarList<DoubleType> &elist, const T &eval) const
{
  const NodeScalarList<DoubleType> &nlist = nmp->GetScalarValues<DoubleType>();

  const ConstEdgeList &edgeList = GetRegion().GetEdgeList();
  elist.resize(edgeList.size());
  for (size_t i = 0; i < edgeList.size(); ++i)
  {
    const Edge &edge = *edgeList[i];
    const size_t ni0 = edge.GetHead()->GetIndex();
    const size_t ni1 = edge.GetTail()->GetIndex();
    elist[i] = eval(nlist[ni0], nlist[ni1]);
  }
}

template <typename DoubleType> template <typename T>
void AverageEdgeModel<DoubleType>::doMath(ConstNodeModelPtr nmp, ConstNodeModelPtr nmp_d, EdgeScalarList<DoubleType> &elist0, EdgeScalarList<DoubleType> &elist1, const T &eval) const
{
  const NodeScalarList<DoubleType> &nlist = nmp->GetScalarValues<DoubleType>();

  const ConstEdgeList &edgeList = GetRegion().GetEdgeList();
  elist0.resize(edgeList.size());
  elist1.resize(edgeList.size());

  //// handle if the derivative model isn't available.  Implied a derivative w.r.t. itself
  if (nmp_d)
  {
    const NodeScalarList<DoubleType> &nlist_d = nmp_d->GetScalarValues<DoubleType>();
    for (size_t i = 0; i < edgeList.size(); ++i)
    {
      const Edge &edge = *edgeList[i];
      const size_t ni0 = edge.GetHead()->GetIndex();
      const size_t ni1 = edge.GetTail()->GetIndex();
      const std::pair<DoubleType, DoubleType> &out = eval(nlist[ni0], nlist_d[ni0], nlist[ni1], nlist_d[ni1]);
      elist0[i] = out.first;
      elist1[i] = out.second;
    }
  }
  else
  {
    for (size_t i = 0; i < edgeList.size(); ++i)
    {
      const Edge &edge = *edgeList[i];
      const size_t ni0 = edge.GetHead()->GetIndex();
      const size_t ni1 = edge.GetTail()->GetIndex();
      static const DoubleType one(1.0);
      const std::pair<DoubleType, DoubleType> &out = eval(nlist[ni0], one, nlist[ni1], one);
      elist0[i] = out.first;
      elist1[i] = out.second;
    }
  }
}

template <typename DoubleType>
void AverageEdgeModel<DoubleType>::doGradient(ConstNodeModelPtr nmp, EdgeScalarList<DoubleType> &elist, DoubleType scl) const
{
  const NodeScalarList<DoubleType> &nlist = nmp->GetScalarValues<DoubleType>();

  const Region &region = GetRegion();
  const ConstEdgeList &edgeList = region.GetEdgeList();
  elist.resize(edgeList.size());

  const ConstEdgeModelPtr em = GetRegion().GetEdgeModel("EdgeInverseLength");
  if (!em) {
    dsErrors::MissingModelModelDependency(region, "EdgeInverseLength", dsErrors::ModelInfo::EDGE, GetName(), dsErrors::ModelInfo::EDGE, OutputStream::OutputType::FATAL);
    return;
  }
  const EdgeScalarList<DoubleType> &invLen = em->GetScalarValues<DoubleType>();

  for (size_t i = 0; i < edgeList.size(); ++i)
  {
    const Edge &edge = *edgeList[i];
    const DoubleType ni0 = nlist[edge.GetHead()->GetIndex()];
    const DoubleType ni1 = nlist[edge.GetTail()->GetIndex()];
    const DoubleType ev  = scl * invLen[i];
    elist[i] = ev * (ni1 - ni0);
  }
}

template <typename DoubleType>
void AverageEdgeModel<DoubleType>::doGradient(ConstNodeModelPtr /*nmp*/, ConstNodeModelPtr nmp_d, EdgeScalarList<DoubleType> &elist0, EdgeScalarList<DoubleType> &elist1, DoubleType scl) const
{
//  const NodeScalarList<DoubleType> &nlist   = nmp->GetScalarValues<DoubleType>();

  const Region &region = GetRegion();
  const ConstEdgeList &edgeList = region.GetEdgeList();
  elist0.resize(edgeList.size());
  elist1.resize(edgeList.size());

  const ConstEdgeModelPtr em = GetRegion().GetEdgeModel("EdgeInverseLength");
  if (!em)
  {
    dsErrors::MissingModelModelDependency(region, "EdgeInverseLength", dsErrors::ModelInfo::EDGE, GetName(), dsErrors::ModelInfo::EDGE, OutputStream::OutputType::FATAL);
    return;
  }
  const EdgeScalarList<DoubleType> &invLen = em->GetScalarValues<DoubleType>();

  if (nmp_d)
  {
    const NodeScalarList<DoubleType> &nlist_d = nmp_d->GetScalarValues<DoubleType>();
    for (size_t i = 0; i < edgeList.size(); ++i)
    {
      const Edge &edge = *edgeList[i];
      const DoubleType ev  = scl * invLen[i];
      const DoubleType ni0d = nlist_d[edge.GetHead()->GetIndex()];
      const DoubleType ni1d = nlist_d[edge.GetTail()->GetIndex()];
      elist0[i] = -ev * ni0d;
      elist1[i] =  ev * ni1d;
    }
  }
  else
  {
    for (size_t i = 0; i < edgeList.size(); ++i)
    {
      const DoubleType ev  = scl * invLen[i];
      elist0[i] = -ev;
      elist1[i] =  ev;
    }
  }
}

AverageEdgeModelEnum::AverageType_t AverageEdgeModelEnum::GetTypeName(const std::string &name, std::string &errorString)
{
  AverageEdgeModelEnum::AverageType_t ret = AverageEdgeModelEnum::UNKNOWN;
  AverageEdgeModelEnum::TypeNameMap_t *typeit = AverageEdgeModelEnum::AverageTypeNames;
  while (typeit->str != nullptr)
  {
    if (typeit->str == name)
    {
      ret = typeit->type;
      break;
    }
    ++typeit;
  }

  if (ret == AverageEdgeModelEnum::UNKNOWN)
  {
    AverageEdgeModelEnum::TypeNameMap_t *typeit = AverageEdgeModelEnum::AverageTypeNames;
    std::ostringstream os;
    os << "\"" << name << "\" is not a valid average type, available options are";
    while (typeit->str != nullptr)
    {
      os << " \"" << typeit->str << "\"";
      ++typeit;
    }
    os << "\n";
    errorString += os.str();
  }
  return ret;
}

template <typename DoubleType>
void AverageEdgeModel<DoubleType>::Serialize(std::ostream &of) const
{
  // originalEdgeModelName is for the derivative case
  of << "COMMAND edge_average_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -node_model \"" << nodeModelName << "\"" << " -edge_model \"" << originalEdgeModelName << "\" -average_type \"" << AverageEdgeModelEnum::AverageTypeNames[averageType].str << "\"";

  if (!variableName.empty())
  {
    of << " -derivative \"" << variableName << "\"";
  }
}

template class AverageEdgeModel<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class AverageEdgeModel<float128>;
#endif

EdgeModelPtr CreateAverageEdgeModel(const std::string &edgemodel, const std::string &nodemodel, AverageEdgeModelEnum::AverageType_t averagetype, RegionPtr rp)
{
  const bool use_extended = rp->UseExtendedPrecisionModels();
  return create_edge_model<AverageEdgeModel<double>, AverageEdgeModel<extended_type>>(use_extended, edgemodel, nodemodel, averagetype, rp);
}

EdgeModelPtr CreateAverageEdgeModel(const std::string &edgemodel, const std::string &nodemodel, const std::string &var, AverageEdgeModelEnum::AverageType_t averagetype, RegionPtr rp)
{
  const bool use_extended = rp->UseExtendedPrecisionModels();
  return create_edge_model<AverageEdgeModel<double>, AverageEdgeModel<extended_type>>(use_extended, edgemodel, nodemodel, var, averagetype, rp);
}



