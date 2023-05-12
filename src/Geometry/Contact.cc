/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "Contact.hh"
#include "Region.hh"
#include "Node.hh"
#include "ContactEquationHolder.hh"
#include "GeometryStream.hh"
#include "dsAssert.hh"
#include "Edge.hh"
#include "Node.hh"
#include "Triangle.hh"
#include "NodeModel.hh"
#include "EdgeModel.hh"
#include <string>
#include <vector>

namespace
{
template <typename T> void deleteMapPointers(std::map<std::string, T *> &x)
{
    typedef std::map<std::string, T *> mtype;

    typename mtype::iterator it = x.begin();
    for ( ; it != x.end(); ++it)
    {
        delete it->second;
    }
}
}

Contact::Contact(const std::string &nm, RegionPtr r, const ConstNodeList_t &cnv, const std::string &mname)
    : name(nm), materialName(mname), region(r), contactnodes(cnv)
{
}

ConstRegionPtr Contact::GetRegion() const
{
    return region;
}

const std::string &Contact::GetName() const
{
    return name;
}

const std::string &Contact::GetMaterialName() const
{
    return materialName;
}

void Contact::SetMaterial(const std::string &new_material)
{
  /*
    for now just invalidate any models that are on the region and on the contact
  */
  const Region::NodeModelList_t &nml = GetRegion()->GetNodeModelList();
  for (Region::NodeModelList_t::const_iterator it = nml.begin(); it != nml.end(); ++it)
  {
    if (&(it->second->GetContact()) == this)
    {
      it->second->MarkOld();
    }
  }
  const Region::EdgeModelList_t &eml = GetRegion()->GetEdgeModelList();
  for (Region::EdgeModelList_t::const_iterator it = eml.begin(); it != eml.end(); ++it)
  {
    if (&(it->second->GetContact()) == this)
    {
      it->second->MarkOld();
    }
  }
  materialName = new_material;
}

const ConstNodeList &Contact::GetNodes() const
{
    return contactnodes;
}

const std::string &Contact::GetDeviceName() const
{
  return GetRegion()->GetDeviceName();
}

Contact::~Contact()
{
#if 0
  deleteMapPointers(contactEquationPtrMap);
#endif
}

///// Copied from region version
// number equations by order they are entered
void Contact::AddEquation(ContactEquationHolder &eq)
{
  // Replace this with a warning
  const std::string nm  = eq.GetName();

  if (contactEquationPtrMap.count(nm))
  {
    ContactEquationHolder &oeq = contactEquationPtrMap[nm];
    if (oeq == eq)
    {
      std::ostringstream os;
      os << "Warning: Will not replace Contact Equation with itself.\n"
          "Region: " << this->GetName() << ", Equation: " << nm <<
          "\n";
      GeometryStream::WriteOut(OutputStream::OutputType::INFO, *this, os.str());
    }
    else
    {
      std::ostringstream os;
      os << "Warning: Replacing Contact Equation with Contact Equation of the same name.\n"
          "Contact: " << this->GetName() << ", Equation: " << nm << "\n";
      GeometryStream::WriteOut(OutputStream::OutputType::INFO, *this, os.str());

      contactEquationPtrMap[nm] = eq;
        /// the contactEquationIndexMap doesn't change
    }
  }
  else
  {
    contactEquationPtrMap[nm] = eq;
  }
}

//// Deletes the equation and its associated variable
//// Decrements the equation index for all other equations
void Contact::DeleteEquation(ContactEquationHolder &eq)
{
  const std::string nm  = eq.GetName();
  dsAssert(contactEquationPtrMap.count(nm) != 0, "UNEXPECTED");
  contactEquationPtrMap.erase(nm);
}

ContactEquationPtrMap_t &Contact::GetEquationPtrList()
{
  return contactEquationPtrMap;
}

const ContactEquationPtrMap_t &Contact::GetEquationPtrList() const
{
  return contactEquationPtrMap;
}

template <typename DoubleType>
void Contact::Assemble(dsMath::RealRowColValueVec<DoubleType> &m, dsMath::RHSEntryVec<DoubleType> &v, PermutationMap &p, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
  for (auto it : GetEquationPtrList())
  {
    it.second.Assemble(m, v, p, w, t);
  }
}

void Contact::FindEdges() const
{
  const Region &region = *(GetRegion());

  const size_t dimension = region.GetDimension();

  if (dimension == 1)
  {
    return;
  }
  else if (dimension ==3)
  {
    FindTriangles();
    return;
  }

  contactedges.clear();

  const ConstEdgeList &el = region.GetEdgeList();

  const Region::EdgeToConstTriangleList_t &ett = region.GetEdgeToTriangleList();

  std::set<size_t> indexes;

  for (size_t i = 0; i < contactnodes.size(); ++i)
  {
    indexes.insert(contactnodes[i]->GetIndex());
  }

  for (size_t i = 0; i < el.size(); ++i)
  {
    const Edge &edge = *el[i];

    //// by definition, only one interface edge can exist in region
    if (ett[edge.GetIndex()].size() != 1)
    {
      continue;
    }

    if ((indexes.find(edge.GetHead()->GetIndex()) != indexes.end())
      && (indexes.find(edge.GetTail()->GetIndex()) != indexes.end())
       )
    {
      contactedges.push_back(&edge);
    }
  }
}

void Contact::FindTriangles() const
{
  contacttriangles.clear();
  contactedges.clear();

  const Region &region = *(GetRegion());

  const ConstTriangleList &tl = region.GetTriangleList();

  const ConstEdgeList &el = region.GetEdgeList();

  const Region::TriangleToConstTetrahedronList_t &ett = region.GetTriangleToTetrahedronList();

  const Region::TriangleToConstEdgeList_t &ete = region.GetTriangleToEdgeList();

  std::set<size_t> indexes;

  std::set<size_t> edge_indexes;

  for (size_t i = 0; i < contactnodes.size(); ++i)
  {
    indexes.insert(contactnodes[i]->GetIndex());
  }

  for (size_t i = 0; i < tl.size(); ++i)
  {
    const Triangle &triangle = *tl[i];

    const size_t triangle_index = triangle.GetIndex();
    if (ett[triangle_index].size() != 1)
    {
      continue;
    }

    const std::vector<ConstNodePtr> &node_list = triangle.GetNodeList();
    if ((indexes.find(node_list[0]->GetIndex()) != indexes.end())
      && (indexes.find(node_list[1]->GetIndex()) != indexes.end())
      && (indexes.find(node_list[2]->GetIndex()) != indexes.end())
      )
    {
      contacttriangles.push_back(&triangle);

      const ConstEdgeList &cel = ete[triangle_index];
      for (size_t i = 0; i < cel.size(); ++i)
      {
        edge_indexes.insert(cel[i]->GetIndex());
      }
    }
  }

  for (std::set<size_t>::const_iterator it = edge_indexes.begin(); it != edge_indexes.end(); ++it)
  {
    contactedges.push_back(el[*it]);
  }
}

const ConstEdgeList_t &Contact::GetEdges() const
{
  if (contactedges.empty())
  {
    FindEdges();
  }
  return contactedges;
}

const ConstTriangleList_t &Contact::GetTriangles() const
{
  if (contacttriangles.empty())
  {
    FindTriangles();
  }
  return contacttriangles;
}

void Contact::AddEdges(const ConstEdgeList &elist)
{
  contactedges = elist;
  region->SignalCallbacks("@@@ContactChange");
}

void Contact::AddTriangles(const ConstTriangleList &tlist)
{
  contacttriangles = tlist;
  region->SignalCallbacks("@@@ContactChange");
}



template void Contact::Assemble(dsMath::RealRowColValueVec<double> &m, dsMath::RHSEntryVec<double> &v, PermutationMap &p, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t);
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template void Contact::Assemble(dsMath::RealRowColValueVec<float128> &m, dsMath::RHSEntryVec<float128> &v, PermutationMap &p, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t);
#endif
