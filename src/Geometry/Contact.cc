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

#include "Contact.hh"
#include "Region.hh"
#include "Node.hh"
#include "ContactEquation.hh"
#include "GeometryStream.hh"
#include "dsAssert.hh"
#include "IterHelper.hh"
#include "Edge.hh"
#include "Node.hh"
#include "Triangle.hh"
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

Contact::Contact(const std::string &nm, RegionPtr r, const ConstNodeList_t &cnv)
    : name(nm), region(r), contactnodes(cnv)
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
  deleteMapPointers(contactEquationPtrMap);
}

///// Copied from region version
// number equations by order they are entered
void Contact::AddEquation(ContactEquationPtr eq)
{
  // Replace this with a warning
  const std::string nm  = eq->GetName();
  const std::string var = eq->GetVariable();

  if (contactEquationPtrMap.count(nm))
  {
    ContactEquationPtr oeq = contactEquationPtrMap[nm];
    if (oeq == eq)
    {
      std::ostringstream os; 
      os << "Warning: Will not replace Contact Equation with itself.\n"
          "Region: " << this->GetName() << ", Equation: " << nm <<
          ", New Variable: " << var << "\n";
      GeometryStream::WriteOut(OutputStream::INFO, *this, os.str());
    }
    else
    {
      if (oeq->GetVariable() != var)
      {
        std::ostringstream os; 
        os << "Warning: Adding a new Contact Equation by the same name with a different variable will remove mapping to other variable.\n"
            "Region: " << this->GetName() << ", Equation: " << nm << ", Old variable: " << oeq->GetVariable() << ", New Variable: " << var << "\n";
        GeometryStream::WriteOut(OutputStream::INFO, *this, os.str());

        variableEquationMap.erase(var);
        variableEquationMap[var] = nm;
      }
      else
      {
        std::ostringstream os; 
        os << "Warning: Replacing Contact Equation with Contact Equation of the same name.\n"
            "Contact: " << this->GetName() << ", Equation: " << nm << ", Variable: " << var << "\n";
        GeometryStream::WriteOut(OutputStream::INFO, *this, os.str());
      }
      delete oeq;
      contactEquationPtrMap[nm] = eq;
        /// the contactEquationIndexMap doesn't change
    }
  }
  else
  {
    if (variableEquationMap.count(var))
    {
      const std::string oenm = variableEquationMap[var];
      std::ostringstream os; 
      os << "ERROR: Can't create Contact Equation if its variable is already being used\n"
          << "New Equation: " << nm << ", Old Equation: " << oenm << ", Variable: "  << var << "\n";
      GeometryStream::WriteOut(OutputStream::ERROR, *this, os.str());
      dsAssert(false, "UNEXPECTED");
    }
    else
    {
      contactEquationPtrMap[nm] = eq;
      variableEquationMap[var] = nm;
    }
  }
}

//// Deletes the equation and its associated variable
//// Decrements the equation index for all other equations
void Contact::DeleteEquation(ContactEquationPtr eq)
{
  // Replace this with a warning
  const std::string nm  = eq->GetName();
  dsAssert(contactEquationPtrMap.count(nm) != 0, "UNEXPECTED");

  const std::string var = eq->GetVariable();
  dsAssert(variableEquationMap.count(var) != 0, "UNEXPECTED");

  contactEquationPtrMap.erase(nm);
  variableEquationMap.erase(var);
}

ContactEquationPtrMap_t &Contact::GetEquationPtrList()
{
  return contactEquationPtrMap;
}

const ContactEquationPtrMap_t &Contact::GetEquationPtrList() const
{
  return contactEquationPtrMap;
}

void Contact::Assemble(dsMath::RealRowColValueVec &m, dsMath::RHSEntryVec &v, PermutationMap &p, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
  IterHelper::ForEachMapValue(GetEquationPtrList(), IterHelper::assdcp<ContactEquation>(m, v, p, w, t));
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
    if ((indexes.find(edge.GetHead()->GetIndex()) != indexes.end())
      && (indexes.find(edge.GetTail()->GetIndex()) != indexes.end())
       )
    {
      //// by definition, only one interface edge can exist in region
      if (ett[edge.GetIndex()].size() == 1)
      {
        contactedges.push_back(&edge);
      }
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
    const std::vector<ConstNodePtr> &node_list = triangle.GetNodeList();
    if ((indexes.find(node_list[0]->GetIndex()) != indexes.end())
      && (indexes.find(node_list[1]->GetIndex()) != indexes.end())
      && (indexes.find(node_list[2]->GetIndex()) != indexes.end())
      )
    {
      const size_t triangle_index = triangle.GetIndex();
      if (ett[triangle_index].size() == 1)
      {
        contacttriangles.push_back(&triangle);

        const ConstEdgeList &cel = ete[triangle_index];
        for (size_t i = 0; i < cel.size(); ++i)
        {
          edge_indexes.insert(cel[i]->GetIndex());
        }
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


