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

AverageEdgeModel::TypeNameMap_t AverageEdgeModel::AverageTypeNames[] = {
        {"arithmetic", ARITHMETIC},
        {"geometric", GEOMETRIC},
        {"gradient", GRADIENT},
        {"negative_gradient", NEGATIVE_GRADIENT},
        {NULL, UNKNOWN}};

AverageEdgeModel::~AverageEdgeModel()
{
  // We can't do this, it causes seg fault if the region is already gone
#if 0
  if (!edgeModel1Name.empty())
  {
    dsErrors::ModelModelDeletion(GetRegion(), edgeModel1Name, dsErrors::ModelInfo::EDGE, GetName(), dsErrors::ModelInfo::EDGE, OutputStream::INFO);
    ///// This is EVIL
    //const_cast<Region &>(GetRegion()).DeleteEdgeModel(edgeModel1Name);
  }
#endif
}

AverageEdgeModel::AverageEdgeModel(const std::string &emodel, const std::string &nmodel, AverageType_t atype, RegionPtr rp)
    :
        EdgeModel(emodel, rp, EdgeModel::SCALAR),
        originalEdgeModelName(emodel),
        nodeModelName(nmodel),
        averageType(atype)
{
    dsAssert(rp->GetNodeModel(nmodel).get(), "UNEXPECTED");
    RegisterCallback(nodeModelName);

    if ((averageType == GRADIENT) || (averageType == NEGATIVE_GRADIENT))
    {
      RegisterCallback("EdgeInverseLength");
    }
}

AverageEdgeModel::AverageEdgeModel(const std::string &emodel, const std::string &nmodel, const std::string &deriv, AverageType_t atype, RegionPtr rp)
    :
        EdgeModel(emodel + ":" + deriv + "@n0", rp, EdgeModel::SCALAR),
        originalEdgeModelName(emodel),
        nodeModelName(nmodel),
        edgeModel1Name(emodel + ":" + deriv + "@n1"),
        variableName(deriv),
        averageType(atype)
{
    node1EdgeModel = EdgeSubModel::CreateEdgeSubModel(edgeModel1Name, rp, EdgeModel::SCALAR, this->GetSelfPtr());
    dsAssert(!node1EdgeModel.expired(), "UNEXPECTED");

    if (!rp->GetNodeModel(nodeModelName))
    {
      dsErrors::MissingModelModelDependency(GetRegion(), nodeModelName, dsErrors::ModelInfo::NODE, GetName(), dsErrors::ModelInfo::EDGE, OutputStream::FATAL);
    }

    RegisterCallback(nodeModelName);

    if (!variableName.empty())
    {
      if (variableName != nodeModelName)
      {
        derivativeModelName = nodeModelName + ":" + variableName;
        if (!rp->GetNodeModel(derivativeModelName))
        {
          dsErrors::MissingModelModelDependency(GetRegion(), derivativeModelName, dsErrors::ModelInfo::NODE, GetName(), dsErrors::ModelInfo::EDGE, OutputStream::FATAL);
        }
        RegisterCallback(derivativeModelName);
      }
    }

    if ((averageType == GRADIENT) || (averageType == NEGATIVE_GRADIENT))
    {
      RegisterCallback("EdgeInverseLength");
    }
#if 0
    os << "creating AverageEdgeModelModel " << edgemodel0 << " with parent " << rp->GetNodeModel(nodemodel)->GetName() << "\n";
#endif
}

namespace {
struct Arithmetic {
  double operator()(double x, double y) const
  {
    return 0.5 * (x + y);
  }

  std::pair<double, double> operator()(double /*x*/, double xd, double /*y*/, double yd) const
  {
    return std::make_pair(0.5 * xd, 0.5 * yd);
  }
};

struct Geometric {
  double operator()(double x, double y) const
  {
    return std::sqrt(x*y);
  }

  //// more efficient would be using the non-derivative model and scaling
  std::pair<double, double> operator()(double x, double xd, double y, double yd) const
  {
    double ratio = sqrt(y/x);
    return std::make_pair(0.5 * xd * ratio, 0.5 * yd /ratio);
  }
};

};

void AverageEdgeModel::calcEdgeScalarValues() const
{
    Region *rp = const_cast<Region *>(&this->GetRegion());

    ConstNodeModelPtr nm = rp->GetNodeModel(nodeModelName);

    ConstNodeModelPtr nmDerivative;
    if (!derivativeModelName.empty())
    {
      nmDerivative = rp->GetNodeModel(derivativeModelName);
      if (!nmDerivative)
      {
        dsErrors::MissingModelModelDependency(*rp, nodeModelName, dsErrors::ModelInfo::NODE, GetName(), dsErrors::ModelInfo::EDGE, OutputStream::ERROR);
      }
    }

    if (!nm)
    {
      dsErrors::MissingModelModelDependency(*rp, nodeModelName, dsErrors::ModelInfo::NODE, GetName(), dsErrors::ModelInfo::EDGE, OutputStream::ERROR);
    }

    if (variableName.empty())
    {
      EdgeScalarList esl;
      switch (averageType) {
          case ARITHMETIC:
            doMath(nm, esl, Arithmetic());
            break;
          case GEOMETRIC:
            doMath(nm, esl, Geometric());
            break;
          case GRADIENT:
            doGradient(nm, esl, 1.0);
            break;
          case NEGATIVE_GRADIENT:
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
      ///TODO: fix complicated logic and simplify
      if (!em1)
      {
        dsErrors::ReviveContainer(*rp, edgeModel1Name, dsErrors::ModelInfo::EDGE, GetName(), dsErrors::ModelInfo::EDGE, OutputStream::INFO);
        node1EdgeModel = EdgeSubModel::CreateEdgeSubModel(edgeModel1Name, rp, EdgeModel::SCALAR, this->GetConstSelfPtr());
      }
      else if (node1EdgeModel.expired())
      {
        dsErrors::ChangedModelModelDependency(*rp, nodeModelName, dsErrors::ModelInfo::NODE, GetName(),      dsErrors::ModelInfo::EDGE, OutputStream::ERROR);
        dsErrors::ChangedModelModelDependency(*rp, nodeModelName, dsErrors::ModelInfo::NODE, edgeModel1Name, dsErrors::ModelInfo::EDGE, OutputStream::FATAL);
      }

      
      EdgeScalarList esl0;
      EdgeScalarList esl1;

      switch (averageType) {
          case ARITHMETIC:
            doMath(nm, nmDerivative, esl0, esl1, Arithmetic());
            break;
          case GEOMETRIC:
            doMath(nm, nmDerivative, esl0, esl1, Geometric());
            break;
          case GRADIENT:
            doGradient(nm, nmDerivative, esl0, esl1, 1.0);
            break;
          case NEGATIVE_GRADIENT:
            doGradient(nm, nmDerivative, esl0, esl1, -1.0);
            break;
          default:
            break;
      }
      this->SetValues(esl0);
      std::const_pointer_cast<EdgeModel, const EdgeModel>(node1EdgeModel.lock())->SetValues(esl1);
    }
}

template <typename T>
void AverageEdgeModel::doMath(ConstNodeModelPtr nmp, EdgeScalarList &elist, const T &eval) const
{
  const NodeScalarList &nlist = nmp->GetScalarValues();

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

template <typename T>
void AverageEdgeModel::doMath(ConstNodeModelPtr nmp, ConstNodeModelPtr nmp_d, EdgeScalarList &elist0, EdgeScalarList &elist1, const T &eval) const
{
  const NodeScalarList &nlist = nmp->GetScalarValues();

  const ConstEdgeList &edgeList = GetRegion().GetEdgeList();
  elist0.resize(edgeList.size());
  elist1.resize(edgeList.size());

  //// handle if the derivative model isn't available.  Implied a derivative w.r.t. itself
  if (nmp_d)
  {
    const NodeScalarList &nlist_d = nmp_d->GetScalarValues();
    for (size_t i = 0; i < edgeList.size(); ++i)
    {
      const Edge &edge = *edgeList[i];
      const size_t ni0 = edge.GetHead()->GetIndex();
      const size_t ni1 = edge.GetTail()->GetIndex();
      const std::pair<double, double> &out = eval(nlist[ni0], nlist_d[ni0], nlist[ni1], nlist_d[ni1]);
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
      const std::pair<double, double> &out = eval(nlist[ni0], 1.0, nlist[ni1], 1.0);
      elist0[i] = out.first;
      elist1[i] = out.second;
    }
  }
}

void AverageEdgeModel::doGradient(ConstNodeModelPtr nmp, EdgeScalarList &elist, double scl) const
{
  const NodeScalarList &nlist = nmp->GetScalarValues();

  const Region &region = GetRegion();
  const ConstEdgeList &edgeList = region.GetEdgeList();
  elist.resize(edgeList.size());

  const ConstEdgeModelPtr em = GetRegion().GetEdgeModel("EdgeInverseLength");
  if (!em) {
    dsErrors::MissingModelModelDependency(region, "EdgeInverseLength", dsErrors::ModelInfo::EDGE, GetName(), dsErrors::ModelInfo::EDGE, OutputStream::FATAL);
  }
  const EdgeScalarList &invLen = em->GetScalarValues();

  for (size_t i = 0; i < edgeList.size(); ++i)
  {
    const Edge &edge = *edgeList[i];
    const double ni0 = nlist[edge.GetHead()->GetIndex()];
    const double ni1 = nlist[edge.GetTail()->GetIndex()];
    const double ev  = scl * invLen[i];
    elist[i] = ev * (ni1 - ni0);
  }
}

///// TODO: optimize this later since we are essentially just using the edge inverse length
///// TODO: maybe not such a big deal since the inverse length would have to change for the values to change
void AverageEdgeModel::doGradient(ConstNodeModelPtr /*nmp*/, ConstNodeModelPtr nmp_d, EdgeScalarList &elist0, EdgeScalarList &elist1, double scl) const
{
//  const NodeScalarList &nlist   = nmp->GetScalarValues();

  const Region &region = GetRegion();
  const ConstEdgeList &edgeList = region.GetEdgeList();
  elist0.resize(edgeList.size());
  elist1.resize(edgeList.size());

  const ConstEdgeModelPtr em = GetRegion().GetEdgeModel("EdgeInverseLength");
  if (!em)
  {
    dsErrors::MissingModelModelDependency(region, "EdgeInverseLength", dsErrors::ModelInfo::EDGE, GetName(), dsErrors::ModelInfo::EDGE, OutputStream::FATAL);
  }
  const EdgeScalarList &invLen = em->GetScalarValues();

  if (nmp_d)
  {
    const NodeScalarList &nlist_d = nmp_d->GetScalarValues();
    for (size_t i = 0; i < edgeList.size(); ++i)
    {
      const Edge &edge = *edgeList[i];
      const double ev  = scl * invLen[i];
      const double ni0d = nlist_d[edge.GetHead()->GetIndex()];
      const double ni1d = nlist_d[edge.GetTail()->GetIndex()];
      elist0[i] = -ev * ni0d;
      elist1[i] =  ev * ni1d;
    }
  }
  else
  {
    for (size_t i = 0; i < edgeList.size(); ++i)
    {
      const double ev  = scl * invLen[i];
      elist0[i] = -ev;
      elist1[i] =  ev;
    }
  }
}

AverageEdgeModel::AverageType_t AverageEdgeModel::GetTypeName(const std::string &name, std::string &errorString)
{
  AverageType_t ret = UNKNOWN;
  AverageEdgeModel::TypeNameMap_t *typeit = AverageTypeNames;
  while (typeit->str != NULL)
  {
    if (typeit->str == name)
    {
      ret = typeit->type;
      break;
    }
    ++typeit;
  }

  if (ret == UNKNOWN)
  {
    AverageEdgeModel::TypeNameMap_t *typeit = AverageTypeNames;
    std::ostringstream os;
    os << "\"" << name << "\" is not a valid average type, available options are";
    while (typeit->str != NULL)
    {
      os << " \"" << typeit->str << "\"";
      ++typeit;
    }
    os << "\n";
    errorString += os.str();
  }
  return ret;
}

void AverageEdgeModel::Serialize(std::ostream &of) const
{
  // originalEdgeModelName is for the derivative case
  of << "COMMAND edge_average_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -node_model \"" << nodeModelName << "\"" << " -edge_model \"" << originalEdgeModelName << "\" -average_type \"" << AverageTypeNames[averageType].str << "\"";

  if (!variableName.empty())
  {
    of << " -derivative \"" << variableName << "\"";
  }
}

