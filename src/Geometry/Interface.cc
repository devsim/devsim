/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "Interface.hh"
#include "InterfaceEquationHolder.hh"
#include "InterfaceNodeModel.hh"
#include "GeometryStream.hh"
#include "dsAssert.hh"
#include "Region.hh"
#include "Node.hh"
#include "Edge.hh"
#include "Triangle.hh"
#include "GlobalData.hh"
#include "ObjectHolder.hh"
#include <set>
#include <sstream>

Interface::Interface(const std::string &nm, Region *r0, Region *r1, const ConstNodeList_t &n0, const ConstNodeList_t &n1)
    : name(nm), rp0(r0), rp1(r1), nodes0(n0), nodes1(n1), elements_provided_(false)
{
}

Interface::~Interface()
{
#if 0
    {
        InterfaceEquationPtrMap_t::iterator it = interfaceEquationList.begin();
        for (;it != interfaceEquationList.end(); ++it)
        {
            delete it->second;
        }
    }
#endif
}

const std::string &Interface::GetDeviceName() const
{
  return rp0->GetDeviceName();
}

/// Here we are taking ownership
void Interface::AddInterfaceEquation(InterfaceEquationHolder &iep)
{
    const std::string &name = iep.GetName();
    InterfaceEquationPtrMap_t::iterator it = interfaceEquationList.find(name);
    if (it == interfaceEquationList.end())
    {
      interfaceEquationList.insert(make_pair(name, iep));
    }
    else
    {
      std::ostringstream os;
      os << "Warning: Replacing interface equation with equation of the same name.\n"
          "Interface: " << this->GetName() << ", Equation: " << name << "\n";
      GeometryStream::WriteOut(OutputStream::OutputType::INFO, *this, os.str());
      interfaceEquationList[name] = iep;
    }
}

void Interface::DeleteInterfaceEquation(InterfaceEquationHolder &iep)
{
    const std::string &name = iep.GetName();
    InterfaceEquationPtrMap_t::iterator it = interfaceEquationList.find(name);
    if (it != interfaceEquationList.end())
    {
      dsAssert(iep == it->second, "UNEXPECTED");
      std::ostringstream os;
      interfaceEquationList.erase(it);
    }
}

const InterfaceEquationPtrMap_t &Interface::GetInterfaceEquationList() const
{
    return interfaceEquationList;
}

InterfaceEquationPtrMap_t &Interface::GetInterfaceEquationList()
{
    return interfaceEquationList;
}

const Interface::NameToInterfaceNodeModelMap_t &Interface::GetInterfaceNodeModelList() const
{
    return interfaceNodeModels;
}


void Interface::AddInterfaceNodeModel(InterfaceNodeModel *nmp)
{
    const std::string &nm = nmp->GetName();
    if (interfaceNodeModels.count(nm))
    {
        std::ostringstream os;
        os << "Replacing Interface Node Model " << nm << " in interface " << name
                  << " of material " << "\n";
        GeometryStream::WriteOut(OutputStream::OutputType::INFO, *this, os.str());
    }

    interfaceNodeModels[nm] = InterfaceNodeModelPtr(nmp);
}

ConstInterfaceNodeModelPtr Interface::GetInterfaceNodeModel(const std::string &nm) const
{
  InterfaceNodeModelPtr em;
  NameToInterfaceNodeModelMap_t::const_iterator it = interfaceNodeModels.find(nm);
  if (interfaceNodeModels.end() != it)
  {
    em = it->second;
  }

  return em;
}

void Interface::DeleteInterfaceNodeModel(const std::string &nm)
{
  NameToInterfaceNodeModelMap_t::iterator it = interfaceNodeModels.find(nm);
  if (it != interfaceNodeModels.end())
  {
    interfaceNodeModels.erase(it);
  }
}

template <typename DoubleType>
void Interface::Assemble(dsMath::RealRowColValueVec<DoubleType> &m, dsMath::RHSEntryVec<DoubleType> &v, PermutationMap &p, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
    for (auto it :  GetInterfaceEquationList())
    {
      it.second.Assemble(m, v, p, w, t);
    }
}

void Interface::RegisterCallback(const std::string &mod, const std::string &dep)
{
    DependencyMap[mod].insert(dep);
}

void Interface::UnregisterCallback(const std::string &mod)
{
    DependencyMap_t::iterator it = DependencyMap.find(mod);
    if (it != DependencyMap.end())
    {
        DependencyMap.erase(it);
    }
}

void Interface::SignalCallbacks(const std::string &str)
{
  typedef std::set<std::string> list_t;
  list_t list;
  DependencyMap_t::iterator it = DependencyMap.begin();
  const DependencyMap_t::iterator end = DependencyMap.end();
  for ( ; it != end; ++it)
  {
    // If this one has a dependency on str
    if ((*it).second.count(str))
    {
      // put it in the set to be notified
      list.insert(it->first);
    }
  }

  list_t::iterator lit = list.begin();
  const list_t::iterator lend = list.end();
  for ( ; lit != lend; ++lit)
  {
    if (interfaceNodeModels.count(*lit))
    {
      InterfaceNodeModelPtr inmp = interfaceNodeModels[*lit];
      if ((inmp->IsUpToDate()))
      {
        inmp->MarkOld();
      }
    }
  }
}

void Interface::SignalCallbacks(const std::string &str, ConstRegionPtr rp)
{
  //// TODO: eventually we need to make it so @r0, @r1 are required (until we get GlobalData on interface
  if (rp == GetRegion0())
  {
    this->SignalCallbacks(str);
    this->SignalCallbacks(str + "@r0");
  }
  else if (rp == GetRegion1())
  {
    this->SignalCallbacks(str);
    this->SignalCallbacks(str + "@r1");
  }
}

void Interface::FindEdges() const
{
  const Region &region0 = *(GetRegion0());
  const Region &region1 = *(GetRegion1());

  dsAssert(region0.GetDimension() == region1.GetDimension(), "Interface region dimension mismatch");
  const size_t dimension = region0.GetDimension();

  if (dimension == 2)
  {
    elements_provided_ = false;
  }
  else if (dimension == 1)
  {
    return;
  }
  else if (dimension ==3)
  {
    FindTriangles();
    return;
  }


  edges0.clear();
  edges1.clear();

  const ConstEdgeList &el0 = region0.GetEdgeList();
  const ConstEdgeList &el1 = region1.GetEdgeList();

  const Region::EdgeToConstTriangleList_t &ett0 = region0.GetEdgeToTriangleList();
  const Region::EdgeToConstTriangleList_t &ett1 = region1.GetEdgeToTriangleList();

  std::set<size_t> indexes0;
  std::set<size_t> indexes1;

  for (size_t i = 0; i < nodes0.size(); ++i)
  {
    indexes0.insert(nodes0[i]->GetIndex());
  }
  for (size_t i = 0; i < nodes1.size(); ++i)
  {
    indexes1.insert(nodes1[i]->GetIndex());
  }

  for (size_t i = 0; i < el0.size(); ++i)
  {
    const Edge &edge = *el0[i];

    //// by definition, only one interface edge can exist in region
    if (ett0[edge.GetIndex()].size() != 1)
    {
      continue;
    }

    if ((indexes0.find(edge.GetHead()->GetIndex()) != indexes0.end())
      && (indexes0.find(edge.GetTail()->GetIndex()) != indexes0.end())
       )
    {
      edges0.push_back(&edge);
    }
  }
  for (size_t i = 0; i < el1.size(); ++i)
  {
    const Edge &edge = *el1[i];

    //// by definition, only one interface edge can exist in region
    if (ett1[edge.GetIndex()].size() != 1)
    {
      continue;
    }

    if ((indexes1.find(edge.GetHead()->GetIndex()) != indexes1.end())
      && (indexes1.find(edge.GetTail()->GetIndex()) != indexes1.end())
       )
    {
      edges1.push_back(&edge);
    }
  }
}

void Interface::FindTriangles() const
{
  triangles0.clear();
  triangles1.clear();
  edges0.clear();
  edges1.clear();

  const Region &region0 = *(GetRegion0());
  const Region &region1 = *(GetRegion1());

  dsAssert(region0.GetDimension() == region1.GetDimension(), "Interface region dimension mismatch");

  elements_provided_ = false;

  const ConstTriangleList &tl0 = region0.GetTriangleList();
  const ConstTriangleList &tl1 = region1.GetTriangleList();

  const ConstEdgeList &el0 = region0.GetEdgeList();
  const ConstEdgeList &el1 = region1.GetEdgeList();

  const Region::TriangleToConstTetrahedronList_t &ett0 = region0.GetTriangleToTetrahedronList();
  const Region::TriangleToConstTetrahedronList_t &ett1 = region1.GetTriangleToTetrahedronList();

  const Region::TriangleToConstEdgeList_t &ete0 = region0.GetTriangleToEdgeList();
  const Region::TriangleToConstEdgeList_t &ete1 = region1.GetTriangleToEdgeList();

  std::set<size_t> indexes0;
  std::set<size_t> indexes1;

  std::set<size_t> edge_indexes0;
  std::set<size_t> edge_indexes1;

  for (size_t i = 0; i < nodes0.size(); ++i)
  {
    indexes0.insert(nodes0[i]->GetIndex());
  }
  for (size_t i = 0; i < nodes1.size(); ++i)
  {
    indexes1.insert(nodes1[i]->GetIndex());
  }

  for (size_t i = 0; i < tl0.size(); ++i)
  {
    const Triangle &triangle = *tl0[i];
    const size_t triangle_index = triangle.GetIndex();
    if (ett0[triangle_index].size() != 1)
    {
      continue;
    }

    const std::vector<ConstNodePtr> &node_list = triangle.GetNodeList();
    if ((indexes0.find(node_list[0]->GetIndex()) != indexes0.end())
      && (indexes0.find(node_list[1]->GetIndex()) != indexes0.end())
      && (indexes0.find(node_list[2]->GetIndex()) != indexes0.end())
    )
    {
      triangles0.push_back(&triangle);

      const ConstEdgeList &cel = ete0[triangle_index];
      for (size_t i = 0; i < cel.size(); ++i)
      {
        edge_indexes0.insert(cel[i]->GetIndex());
      }
    }
  }
  for (size_t i = 0; i < tl1.size(); ++i)
  {
    const Triangle &triangle = *tl1[i];
    const size_t triangle_index = triangle.GetIndex();
    if (ett1[triangle_index].size() != 1)
    {
      continue;
    }

    const std::vector<ConstNodePtr> &node_list = triangle.GetNodeList();
    if ((indexes1.find(node_list[0]->GetIndex()) != indexes1.end())
      && (indexes1.find(node_list[1]->GetIndex()) != indexes1.end())
      && (indexes1.find(node_list[2]->GetIndex()) != indexes1.end())
    )
    {
      triangles1.push_back(&triangle);

      const ConstEdgeList &cel = ete1[triangle_index];
      for (size_t i = 0; i < cel.size(); ++i)
      {
        edge_indexes1.insert(cel[i]->GetIndex());
      }
    }
  }

  for (std::set<size_t>::const_iterator it = edge_indexes0.begin(); it != edge_indexes0.end(); ++it)
  {
    edges0.push_back(el0[*it]);
  }
  for (std::set<size_t>::const_iterator it = edge_indexes1.begin(); it != edge_indexes1.end(); ++it)
  {
    edges1.push_back(el1[*it]);
  }
}

const Interface::ConstEdgeList_t &Interface::GetEdges0() const
{
  if (edges0.empty())
  {
    FindEdges();
  }
  return edges0;
}

const Interface::ConstEdgeList_t &Interface::GetEdges1() const
{
  if (edges1.empty())
  {
    FindEdges();
  }
  return edges1;
}

const Interface::ConstTriangleList_t &Interface::GetTriangles0() const
{
  if (triangles0.empty())
  {
    FindTriangles();
  }
  return triangles0;
}

const Interface::ConstTriangleList_t &Interface::GetTriangles1() const
{
  if (triangles1.empty())
  {
    FindTriangles();
  }
  return triangles1;
}

std::string Interface::GetSurfaceAreaModel() const
{
  const GlobalData &ginst = GlobalData::GetInstance();

  GlobalData::DBEntry_t dbent0 = ginst.GetDBEntryOnRegion(GetRegion0(), "surface_area_model");
  GlobalData::DBEntry_t dbent1 = ginst.GetDBEntryOnRegion(GetRegion1(), "surface_area_model");

  dsAssert(dbent0.first, "surface_area_model not specified\n");
  dsAssert(dbent1.first, "surface_area_model not specified\n");

  const std::string &model0 = dbent0.second.GetString();
  const std::string &model1 = dbent1.second.GetString();

  if (model0 != model1)
  {
    std::string errorString = std::string("surface_area_model in region 0 and region 1 of interface ") + GetName() + " does not match " + model0 + " " + model1 + "\n";
  dsAssert(0, errorString.c_str());
  }

  return model0;
}

template <>
InterfaceModelExprDataCachePtr<double> Interface::GetInterfaceModelExprDataCache()
{
  return interfaceModelExprDataCache_double.lock();
}

template <>
void Interface::SetInterfaceModelExprDataCache(InterfaceModelExprDataCachePtr<double> p)
{
  interfaceModelExprDataCache_double = p;
}

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
InterfaceModelExprDataCachePtr<float128> Interface::GetInterfaceModelExprDataCache()
{
  return interfaceModelExprDataCache_float128.lock();
}

template <>
void Interface::SetInterfaceModelExprDataCache(InterfaceModelExprDataCachePtr<float128> p)
{
  interfaceModelExprDataCache_float128 = p;
}
#endif

void Interface::AddEdges(const ConstEdgeList &elist0, const ConstEdgeList &elist1)
{
  edges0 = elist0;
  edges1 = elist1;
  rp0->SignalCallbacks("@@@InterfaceChange");
  rp1->SignalCallbacks("@@@InterfaceChange");
  if (rp0->GetDimension() == 2)
  {
    elements_provided_ = true;
  }
}

void Interface::AddTriangles(const ConstTriangleList &tlist0, const ConstTriangleList &tlist1)
{
  triangles0 = tlist0;
  triangles1 = tlist1;
  rp0->SignalCallbacks("@@@InterfaceChange");
  rp1->SignalCallbacks("@@@InterfaceChange");
  elements_provided_ = true;
}

bool Interface::UseExtendedPrecisionType(const std::string &t) const
{
  bool ret = false;
#ifdef DEVSIM_EXTENDED_PRECISION
  const GlobalData &ginst = GlobalData::GetInstance();
  GlobalData::DBEntry_t dbent0 = ginst.GetDBEntryOnRegion(GetRegion0(), t);
  GlobalData::DBEntry_t dbent1 = ginst.GetDBEntryOnRegion(GetRegion1(), t);
  if (dbent0.first)
  {
    const auto &x = dbent0.second.GetBoolean();
    if (x.first)
    {
      ret = x.second;
    }
  }
  if (dbent1.first)
  {
    const auto &x = dbent1.second.GetBoolean();
    if (x.first)
    {
      ret = ret || x.second;
    }
  }
#endif
  return ret;
}

bool Interface::UseExtendedPrecisionModels() const
{
  return UseExtendedPrecisionType("extended_model");
}

bool Interface::UseExtendedPrecisionEquations() const
{
  return UseExtendedPrecisionType("extended_equation");
}

#if 0
// Specialized above
template InterfaceModelExprDataCachePtr<double> Interface::GetInterfaceModelExprDataCache();
template void Interface::SetInterfaceModelExprDataCache(InterfaceModelExprDataCachePtr<double> p);
#endif
template void Interface::Assemble(dsMath::RealRowColValueVec<double> &m, dsMath::RHSEntryVec<double> &v, PermutationMap &p, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t);

#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
#if 0
template InterfaceModelExprDataCachePtr<float128> Interface::GetInterfaceModelExprDataCache();
template void Interface::SetInterfaceModelExprDataCache(InterfaceModelExprDataCachePtr<float128> p);
#endif
template void Interface::Assemble(dsMath::RealRowColValueVec<float128> &m, dsMath::RHSEntryVec<float128> &v, PermutationMap &p, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t);
#endif



