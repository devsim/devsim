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

#include "ContactEquation.hh"
#include "Device.hh"
#include "Region.hh"
#include "Node.hh"
#include "Edge.hh"
#include "Triangle.hh"
#include "Tetrahedron.hh"
#include "Contact.hh"
#include "NodeModel.hh"
#include "EdgeModel.hh"
#include "NodeScalarData.hh"
#include "EdgeScalarData.hh"
#include "TriangleEdgeScalarData.hh"
#include "TetrahedronEdgeScalarData.hh"
#include "MatrixEntries.hh"
#include "Permutation.hh"
#include "NodeKeeper.hh"
#include "GeometryStream.hh"
#include "dsAssert.hh"
#include "EdgeData.hh"
#include "EquationErrors.hh"

#include <algorithm>
#include <sstream>
#include <string>

namespace {
//// TODO: inefficient since linear search of same list over and over again
bool bothNodesOnContact(const ConstNodeList_t &cnodes, const Edge &edge)
{
  bool ret = false;
  if (
    (std::find(cnodes.begin(), cnodes.end(), edge.GetHead()) != cnodes.end())
      &&
    (std::find(cnodes.begin(), cnodes.end(), edge.GetTail()) != cnodes.end())
  )
  {
    ret = true;
  }
  return ret;
}
}

const std::string &ContactEquation::GetContactName() const
{
  return GetContact().GetName();
}

const std::string &ContactEquation::GetDeviceName() const
{
  return GetContact().GetDeviceName();
}

void ContactEquation::UpdateContact()
{
  calcCurrent();
  calcCharge();
}

ContactEquation::ContactEquation(const std::string &nm, const std::string &var,
        ContactPtr cp, RegionPtr rp)
    : myname(nm), variable(var), mycontact(cp), myregion(rp), charge(0.0), current(0.0)
{
    cp->AddEquation(this);
}

/////
///// If a node is being used by another contact on the same region by an equation of the same name, we don't use it here
/////
///// TODO: regress this case for two equations of the same name from two different contacts in the same region
ConstNodeList_t ContactEquation::GetActiveNodes() const
{
  ConstNodeList_t ret;

  const Contact &contact = GetContact();

  const std::string &cname = contact.GetName();

  const ConstNodeList_t &cnodes = contact.GetNodes();

  const Region &region = GetRegion();

  const Device &device = *region.GetDevice();

  const Device::CoordinateIndexToContact_t &ctc = device.GetCoordinateIndexToContact();

  const std::string &eqname = GetName();

  for (ConstNodeList_t::const_iterator it = cnodes.begin(); it != cnodes.end(); ++it)
  {

    bool addToList = true;

    const size_t ci = (*it)->GetCoordinate().GetIndex();

    Device::CoordinateIndexToContact_t::const_iterator cit = ctc.find(ci);

    dsAssert(cit != ctc.end(), "UNEXPECTED");

    {
      std::vector<ContactPtr>::const_iterator vit = (cit->second).begin();

      for ( ; vit != (cit->second).end(); ++vit)
      {
        const Contact &ocontact = *(*vit);
        if ((*(ocontact.GetRegion())) == region)
        {
          if (ocontact.GetName() == cname)
          {
            // if we hit this contact first we are cool
            addToList = true;
            break;
          }
          else
          {

            const Contact::ContactEquationPtrMap_t &cepm = contact.GetEquationPtrList();

#if 0
//// We now only support one contact per region
            std::pair<Region::ContactEquationPtrMap_t::const_iterator, Region::ContactEquationPtrMap_t::const_iterator> cpair = cepm.equal_range(cname);

            Region::ContactEquationPtrMap_t::const_iterator cepmit = cpair.first;
            Region::ContactEquationPtrMap_t::const_iterator cepmend = cpair.second;
#endif
            for (Contact::ContactEquationPtrMap_t::const_iterator cepmit = cepm.begin(); cepmit != cepm.end(); ++cepmit)
            {
              if ((cepmit->second)->GetName() == eqname)
              {
                addToList = false;
                break;
              }
            }
            if (!addToList)
            {
              break;
            } 
          }
        }
      }
    }
    if (addToList)
    {
      ret.push_back(*it);
    }
  }
  return ret;
}

void ContactEquation::Assemble(dsMath::RealRowColValueVec &m, RHSEntryVec &v, PermutationMap &p, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
  DerivedAssemble(m, v, p, w, t);
}

ContactEquation::~ContactEquation()
{
}

void ContactEquation::SetCircuitNode(const std::string &n)
{
  circuitnode = n;
}

const std::string &ContactEquation::GetCircuitNode() const
{
  return circuitnode;
}

double ContactEquation::integrateNodeModelOverNodes(const std::string &nmodel, const std::string &node_volume)
{
  double ch = 0.0;

  const ConstNodeList_t &cnodes = GetActiveNodes();

  const Region &region = GetRegion();

  if (!nmodel.empty())
  {
    ConstNodeModelPtr nm = region.GetNodeModel(nmodel);
//    dsAssert(nm != NULL, "UNEXPECTED");

    if (!nm)
    {
      dsErrors::MissingContactEquationModel(region, *this, nmodel, dsErrors::ModelInfo::NODE, OutputStream::FATAL);
    }

    ConstNodeModelPtr nv = region.GetNodeModel(node_volume);
//    dsAssert(nv != NULL, "UNEXPECTED");
    if (!nv)
    {
      dsErrors::MissingContactEquationModel(region, *this, node_volume, dsErrors::ModelInfo::NODE, OutputStream::FATAL);
    }

    NodeScalarData nsd(*nv);
    nsd *= *nm;

    for (ConstNodeList_t::const_iterator cit = cnodes.begin(); cit != cnodes.end(); ++cit)
    {
      const double nodeval = nsd[(*cit)->GetIndex()];
      ch += nodeval;
    }
  }
  return ch;
}

double ContactEquation::integrateEdgeModelOverNodes(const std::string &emodel, const std::string &edge_couple)
{
  double ch = 0.0;

  const ConstNodeList_t &cnodes = GetActiveNodes();

  const Region &region = GetRegion();

  if (!emodel.empty())
  {
    ConstEdgeModelPtr em = region.GetEdgeModel(emodel);
//    dsAssert(em != NULL, "UNEXPECTED");
    if (!em)
    {
      dsErrors::MissingContactEquationModel(region, *this, emodel, dsErrors::ModelInfo::EDGE, OutputStream::FATAL);
    }

    ConstEdgeModelPtr ec = region.GetEdgeModel(edge_couple);
//    dsAssert(ec != NULL, "UNEXPECTED");

    if (!ec)
    {
      dsErrors::MissingContactEquationModel(region, *this, edge_couple, dsErrors::ModelInfo::EDGE, OutputStream::FATAL);
    }

    EdgeScalarData esd(*ec);
    esd *= *em;

    const Region::NodeToConstEdgeList_t &ntelist = region.GetNodeToEdgeList();

    for (ConstNodeList_t::const_iterator cit = cnodes.begin(); cit != cnodes.end(); ++cit)
    {
      //// TODO: should we ignore edges along the contact?
      const ConstEdgeList &el = ntelist[(*cit)->GetIndex()];
      ConstEdgeList::const_iterator it = el.begin();
      const ConstEdgeList::const_iterator itend = el.end();
      for ( ; it != itend; ++it)
      {
        if (bothNodesOnContact(cnodes, **it))
        {
          continue;
        }

        double val = (*it)->GetNodeSign(*cit);
        val *= esd[(*it)->GetIndex()];
        ch += val;
      }
    }
  }
  return ch;
}

//// TODO: worry about CylindricalNodeVolume being assymetric later on
double ContactEquation::integrateTriangleEdgeModelOverNodes(const std::string &emodel, const std::string &edge_couple, const double n0_sign, const double n1_sign)
{
  double ch = 0.0;

  const ConstNodeList_t &cnodes = GetActiveNodes();

  const Region &region = GetRegion();

  if (!emodel.empty())
  {
    ConstTriangleEdgeModelPtr em = region.GetTriangleEdgeModel(emodel);
//    dsAssert(em != NULL, "UNEXPECTED");
    if (!em)
    {
      dsErrors::MissingContactEquationModel(region, *this, emodel, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::FATAL);
    }

    ConstTriangleEdgeModelPtr ec = region.GetTriangleEdgeModel(edge_couple);
//    dsAssert(ec != NULL, "UNEXPECTED");
    if (!ec)
    {
      dsErrors::MissingContactEquationModel(region, *this, edge_couple, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::FATAL);
    }

    TriangleEdgeScalarData esd(*ec);
    esd *= *em;

    const Region::TriangleToConstEdgeList_t &ttelist = region.GetTriangleToEdgeList();
    const Region::NodeToConstTriangleList_t &nttlist = region.GetNodeToTriangleList();

    for (ConstNodeList_t::const_iterator cit = cnodes.begin(); cit != cnodes.end(); ++cit)
    {
      const ConstTriangleList &ntl = nttlist[(*cit)->GetIndex()];
      for (ConstTriangleList::const_iterator ti = ntl.begin(); ti != ntl.end(); ++ti)
      {
        const Triangle &triangle = **ti;
        const size_t tindex = triangle.GetIndex();
        const ConstEdgeList &edgeList = ttelist[tindex];
        for (size_t eindex = 0; eindex < edgeList.size(); ++eindex)
        {
          const Edge &edge = *edgeList[eindex];
          if ((edge.GetHead() == (*cit)) || (edge.GetTail() == (*cit)))
          {
            if (bothNodesOnContact(cnodes, edge))
            {
              continue;
            }

            double val;
            if (*cit == edge.GetHead())
            {
              val = n0_sign;
            }
            else
            {
              val = n1_sign;
            }

            val *= esd[3 * tindex + eindex];
            ch += val;
          } 
        }
      }
    }
  }
  return ch;
}

double ContactEquation::integrateTetrahedronEdgeModelOverNodes(const std::string &emodel, const std::string &edge_couple, const double n0_sign, const double n1_sign)
{
  double ch = 0.0;

  const ConstNodeList_t &cnodes = GetActiveNodes();

  const Region &region = GetRegion();

  if (!emodel.empty())
  {
    ConstTetrahedronEdgeModelPtr em = region.GetTetrahedronEdgeModel(emodel);
//    dsAssert(em != NULL, "UNEXPECTED");
    if (!em)
    {
      dsErrors::MissingContactEquationModel(region, *this, emodel, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::FATAL);
    }

    ConstTetrahedronEdgeModelPtr ec = region.GetTetrahedronEdgeModel(edge_couple);
//    dsAssert(ec != NULL, "UNEXPECTED");
    if (!ec)
    {
      dsErrors::MissingContactEquationModel(region, *this, edge_couple, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::FATAL);
    }

    TetrahedronEdgeScalarData esd(*ec);
    esd *= *em;

    const Region::TetrahedronToConstEdgeDataList_t &ttelist = region.GetTetrahedronToEdgeDataList();
    const Region::NodeToConstTetrahedronList_t &nttlist = region.GetNodeToTetrahedronList();

    for (ConstNodeList_t::const_iterator cit = cnodes.begin(); cit != cnodes.end(); ++cit)
    {
      const ConstTetrahedronList &ntl = nttlist[(*cit)->GetIndex()];
      for (ConstTetrahedronList::const_iterator ti = ntl.begin(); ti != ntl.end(); ++ti)
      {
        const Tetrahedron &tetrahedron = **ti;
        const size_t tindex = tetrahedron.GetIndex();
        const ConstEdgeDataList &edgeDataList = ttelist[tindex];
        for (size_t eindex = 0; eindex < edgeDataList.size(); ++eindex)
        {
          const Edge &edge = *(edgeDataList[eindex]->edge);
          if ((edge.GetHead() == (*cit)) || (edge.GetTail() == (*cit)))
          {
            if (bothNodesOnContact(cnodes, edge))
            {
              continue;
            }

            double val;
            if (*cit == edge.GetHead())
            {
              val = n0_sign;
            }
            else
            {
              val = n1_sign;
            }

            val *= esd[6 * tindex + eindex];
            ch += val;
          } 
        }
      }
    }
  }
  return ch;
}

double ContactEquation::integrateElementEdgeModelOverNodes(const std::string &emodel, const std::string &edge_couple, const double n0_sign, const double n1_sign)
{
  const size_t dimension = GetRegion().GetDimension();

  double ret = 0.0;

  if (dimension == 2)
  {
    ret = integrateTriangleEdgeModelOverNodes(emodel, edge_couple, n0_sign, n1_sign);
  }
  else if (dimension == 3)
  {
    ret = integrateTetrahedronEdgeModelOverNodes(emodel, edge_couple, n0_sign, n1_sign);
  }

  return ret;
}

void ContactEquation::AssembleNodeEquation(const std::string &nmodel, dsMath::RealRowColValueVec &m, RHSEntryVec &v, PermutationMap &p, dsMathEnum::WhatToLoad w, const std::string &node_volume)
{
  dsAssert(!nmodel.empty(), "UNEXPECTED");

  const ConstNodeList_t cnodes = GetActiveNodes();

  const Region &region = GetRegion();

  const size_t eqindex = region.GetEquationIndex(GetName());
//  dsAssert(eqindex != size_t(-1), "UNEXPECTED");
  if (eqindex == size_t(-1))
  {
    dsErrors::MissingContactEquationIndex(region, *mycontact, GetName(), OutputStream::FATAL);
  }


  //// Perhaps this helps scaling
  ConstNodeModelPtr nv = region.GetNodeModel(node_volume);
//  dsAssert(nv != NULL, "UNEXPECTED");
  if (!nv)
  {
    dsErrors::MissingContactEquationModel(region, *this, node_volume, dsErrors::ModelInfo::NODE, OutputStream::FATAL);
  }

  size_t ccol = size_t(-1);
  if (!circuitnode.empty())
  {
    NodeKeeper &nk = NodeKeeper::instance();
    if (nk.IsCircuitNode(circuitnode))
    {
      ccol = nk.GetEquationNumber(circuitnode);
    }
    else
    {
      dsErrors::MissingCircuitNodeOnContactEquation(*this, circuitnode, OutputStream::FATAL);
    }
  }

  if (w == dsMathEnum::PERMUTATIONSONLY)
  {
    for (ConstNodeList_t::const_iterator cit = cnodes.begin(); cit != cnodes.end(); ++cit)
    {
      const size_t row = region.GetEquationNumber(eqindex, *cit);
      // Permutation of the original bulk equation to nowhere
      p[row] = PermutationEntry(size_t(-1));
    }
  }

  if ((w == dsMathEnum::RHS) || (w == dsMathEnum::MATRIXANDRHS))
  {
    ConstNodeModelPtr nm = region.GetNodeModel(nmodel);
//    dsAssert(nm != NULL, "UNEXPECTED");
    if (!nm)
    {
      dsErrors::MissingContactEquationModel(region, *this, nmodel, dsErrors::ModelInfo::NODE, OutputStream::FATAL);
    }

    NodeScalarData nsd(*nv);
    nsd *= *nm;

    for (ConstNodeList_t::const_iterator cit = cnodes.begin(); cit != cnodes.end(); ++cit)
    {
      const size_t row = region.GetEquationNumber(eqindex, *cit);

      double rhsval = nsd.GetScalarList()[(*cit)->GetIndex()];

      v.push_back(std::make_pair(row,  rhsval));
    }
  }

  if ((w == dsMathEnum::MATRIXONLY) || (w == dsMathEnum::MATRIXANDRHS))
  {
    const VariableList_t &vlist = region.GetVariableList();

    for (VariableList_t::const_iterator it = vlist.begin(); it != vlist.end(); ++it)
    {
      const std::string var(*it);
      std::string dermodel = nmodel;
      dermodel += ":";  // should use function to create name for derivative model
      dermodel += var;

      ConstNodeModelPtr ndm = region.GetNodeModel(dermodel);

      if (!ndm)
      {
        dsErrors::MissingContactEquationModel(region, *this, dermodel, dsErrors::ModelInfo::EDGE, OutputStream::VERBOSE1);
      }
      else
      {
        NodeScalarData ndd(*nv);
        ndd *= *ndm;

        const size_t eqindex2 = region.GetEquationIndex(region.GetEquationNameFromVariable(var));
//        dsAssert(eqindex2 != size_t(-1), "UNEXPECTED");
        if (eqindex2 == size_t(-1))
        {
          dsErrors::MissingEquationIndex(region, myname, var, OutputStream::FATAL) ;
        }

        for (ConstNodeList_t::const_iterator cit = cnodes.begin(); cit != cnodes.end(); ++cit)
        {
          const size_t row = region.GetEquationNumber(eqindex, *cit);
          const size_t col = region.GetEquationNumber(eqindex2, (*cit));
          const double val = ndd.GetScalarList()[(*cit)->GetIndex()];

          m.push_back(dsMath::RealRowColVal(row, col, val));
        }
      }
    }

    if (ccol != size_t(-1))
    {
      std::string dermodel = nmodel;
      dermodel += ":";  // should use function to create name for derivative model
      dermodel += circuitnode;

      ConstNodeModelPtr ndm = region.GetNodeModel(dermodel);

      if (!ndm)
      {
      }
      else
      {
        NodeScalarData ndd(*nv);
        ndd *= *ndm;

        for (ConstNodeList_t::const_iterator cit = cnodes.begin(); cit != cnodes.end(); ++cit)
        {
          const size_t row = region.GetEquationNumber(eqindex, *cit);
          const double val = ndd.GetScalarList()[(*cit)->GetIndex()];

          m.push_back(dsMath::RealRowColVal(row, ccol, val));
        }
      }
    }
  }
}

void ContactEquation::AssembleNodeEquationOnCircuit(const std::string &nmodel, dsMath::RealRowColValueVec &m, RHSEntryVec &v, dsMathEnum::WhatToLoad w, const std::string &node_volume)
{
  typedef std::vector<std::string> VariableList_t;
  dsAssert(!nmodel.empty(), "UNEXPECTED");
  dsAssert(!circuitnode.empty(), "UNEXPECTED");

  const ConstNodeList_t cnodes = GetActiveNodes();
  const Region &region = GetRegion();

  size_t crow = size_t(-1);
  NodeKeeper &nk = NodeKeeper::instance();
  if (nk.IsCircuitNode(circuitnode))
  {
    crow = nk.GetEquationNumber(circuitnode);
  }
  else
  {
    dsErrors::MissingCircuitNodeOnContactEquation(*this, circuitnode, OutputStream::FATAL);
  }

  //////
  //////  End execution here
  //////
  if (crow == size_t(-1))
  {
    return;
  }

  ConstNodeModelPtr nv = region.GetNodeModel(node_volume);
//  dsAssert(nv != NULL, "UNEXPECTED");
  if (!nv)
  {
    dsErrors::MissingContactEquationModel(region, *this, node_volume, dsErrors::ModelInfo::NODE, OutputStream::FATAL);
  }

//    os << "DEBC5: circuit node " << nmodel << " " << GetContact().GetName() << " " << GetName() << " " << circuitnode << "\n";
  if ((w == dsMathEnum::RHS) || (w == dsMathEnum::MATRIXANDRHS))
  {
    ConstNodeModelPtr nm = region.GetNodeModel(nmodel);
//    dsAssert(nm != NULL, "UNEXPECTED");
    if (!nm)
    {
      dsErrors::MissingContactEquationModel(region, *this, nmodel, dsErrors::ModelInfo::NODE, OutputStream::FATAL);
    }


    NodeScalarData nsd(*nv);
    nsd *= *nm;

    for (ConstNodeList_t::const_iterator cit = cnodes.begin(); cit != cnodes.end(); ++cit)
    {
      const double rhsval = nsd.GetScalarList()[(*cit)->GetIndex()];
      v.push_back(std::make_pair(crow,  rhsval));
    }
  }

  if ((w == dsMathEnum::MATRIXONLY) || (w == dsMathEnum::MATRIXANDRHS))
  {
    const VariableList_t &vlist = region.GetVariableList();
    for (VariableList_t::const_iterator it = vlist.begin(); it != vlist.end(); ++it)
    {
      const std::string var(*it);
      std::string dermodel = nmodel;
      dermodel += ":";  // should use function to create name for derivative model
      dermodel += var;

      ConstNodeModelPtr ndm = region.GetNodeModel(dermodel);

      if (!ndm)
      {
        dsErrors::MissingContactEquationModel(region, *this, dermodel, dsErrors::ModelInfo::EDGE, OutputStream::VERBOSE1);
      }
      else
      {
        NodeScalarData ndd(*nv);
        ndd *= *ndm;
        for (ConstNodeList_t::const_iterator cit = cnodes.begin(); cit != cnodes.end(); ++cit)
        {
          const size_t eqindex2 = region.GetEquationIndex(region.GetEquationNameFromVariable(var));
//          dsAssert(eqindex2 != size_t(-1), "UNEXPECTED");
          if (eqindex2 == size_t(-1))
          {
            dsErrors::MissingEquationIndex(region, myname, var, OutputStream::FATAL) ;
          }

          const size_t col = region.GetEquationNumber(eqindex2, (*cit));

          const double val = ndd.GetScalarList()[(*cit)->GetIndex()];

          m.push_back(dsMath::RealRowColVal(crow, col, val));
        }
      }
    }

    {
      std::string dermodel = nmodel;
      dermodel += ":" + circuitnode;
      ConstNodeModelPtr ndm = region.GetNodeModel(dermodel);
      if (!ndm)
      {
        dsErrors::MissingContactEquationModel(region, *this, dermodel, dsErrors::ModelInfo::EDGE, OutputStream::VERBOSE1);
      }
      else
      {
        NodeScalarData ndd(*nv);
        ndd *= *ndm;

        for (ConstNodeList_t::const_iterator cit = cnodes.begin(); cit != cnodes.end(); ++cit)
        {
          const double val = ndd.GetScalarList()[(*cit)->GetIndex()];
          m.push_back(dsMath::RealRowColVal(crow, crow, val));
        }
      }
    }
  }
}

void ContactEquation::AssembleEdgeEquation(const std::string &emodel, dsMath::RealRowColValueVec &m, RHSEntryVec &v, dsMathEnum::WhatToLoad w, const std::string &edge_couple)
{
  typedef std::vector<std::string> VariableList_t;

  const ConstNodeList_t cnodes = GetActiveNodes();
  const Region &region = GetRegion();

  const size_t eqindex = region.GetEquationIndex(GetName());
//  dsAssert(eqindex != size_t(-1), "UNEXPECTED");
  if (eqindex == size_t(-1))
  {
    dsErrors::MissingContactEquationIndex(region, *mycontact, GetName(), OutputStream::FATAL) ;
  }


  dsAssert(!emodel.empty(), "UNEXPECTED");

  //// Perhaps this helps scaling
  ConstEdgeModelPtr ec = region.GetEdgeModel(edge_couple);
//  dsAssert(ec != NULL, "UNEXPECTED");
  if (!ec)
  {
    dsErrors::MissingContactEquationModel(region, *this, edge_couple, dsErrors::ModelInfo::EDGE, OutputStream::FATAL);
  }

  size_t ccol = size_t(-1);
  if (!circuitnode.empty())
  {
//      os << "DEBC2: circuit node " << emodel << " " << GetContact().GetName() << " " << GetName() << " " << circuitnode << "\n";
    NodeKeeper &nk = NodeKeeper::instance();
    if (nk.IsCircuitNode(circuitnode))
    {
        ccol = nk.GetEquationNumber(circuitnode);
    }
    else
    {
      dsErrors::MissingCircuitNodeOnContactEquation(*this, circuitnode, OutputStream::FATAL);
    }
  }

  if ((w == dsMathEnum::RHS) || (w == dsMathEnum::MATRIXANDRHS))
  {
    ConstEdgeModelPtr em = region.GetEdgeModel(emodel);
//    dsAssert(em != NULL, "UNEXPECTED");
    if (!em)
    {
      dsErrors::MissingContactEquationModel(region, *this, emodel, dsErrors::ModelInfo::EDGE, OutputStream::FATAL);
    }

    EdgeScalarData esd(*ec);
    esd *= *em;

    const Region::NodeToConstEdgeList_t &ntelist = region.GetNodeToEdgeList();

    for (ConstNodeList_t::const_iterator cit = cnodes.begin(); cit != cnodes.end(); ++cit)
    {
      const ConstEdgeList &el = ntelist[(*cit)->GetIndex()];
      const ConstEdgeList::const_iterator itend = el.end();
      for (ConstEdgeList::const_iterator it = el.begin() ; it != itend; ++it)
      {
        const Node *h = (*it)->GetHead();
        const Node *t = (*it)->GetTail();
        const size_t rowh = region.GetEquationNumber(eqindex, h);
        const size_t rowt = region.GetEquationNumber(eqindex, t);

        double val = esd[(*it)->GetIndex()];
        v.push_back(std::make_pair(rowh,  val));
        v.push_back(std::make_pair(rowt, -val));
      }
    }
  }

  if ((w == dsMathEnum::MATRIXONLY) || (w == dsMathEnum::MATRIXANDRHS))
  {
    const VariableList_t &vlist = region.GetVariableList();
    for (VariableList_t::const_iterator it = vlist.begin(); it != vlist.end(); ++it)
    {
      const std::string var(*it);
      std::string dermodel = emodel;
      dermodel += ":";  // should use function to create name for derivative model
      dermodel += var;

      std::string dermodel0 = dermodel + "@n0";
      std::string dermodel1 = dermodel + "@n1";

      ConstEdgeModelPtr edm0 = region.GetEdgeModel(dermodel0);
      ConstEdgeModelPtr edm1 = region.GetEdgeModel(dermodel1);

      if (!edm0)
      {
        dsErrors::MissingContactEquationModel(region, *this, dermodel0, dsErrors::ModelInfo::EDGE, OutputStream::VERBOSE1);
      }
      else
      {
        EdgeScalarData edd0(*ec);
        EdgeScalarData edd1(*ec);
        edd0 *= *edm0;
        edd1 *= *edm1;

        const size_t eqindex2 = region.GetEquationIndex(region.GetEquationNameFromVariable(var));
//        dsAssert(eqindex2 != size_t(-1), "UNEXPECTED");
        if (eqindex2 == size_t(-1))
        {
          dsErrors::MissingEquationIndex(region, myname, var, OutputStream::FATAL) ;
        }

        const Region::NodeToConstEdgeList_t &ntelist = region.GetNodeToEdgeList();

        for (ConstNodeList_t::const_iterator cit = cnodes.begin() ; cit != cnodes.end(); ++cit)
        {
          const ConstEdgeList &el = ntelist[(*cit)->GetIndex()];
          const ConstEdgeList::const_iterator itend = el.end();
          for (ConstEdgeList::const_iterator it = el.begin() ; it != itend; ++it)
          {
            const Node *h = (*it)->GetHead();
            const Node *t = (*it)->GetTail();
            const size_t rowh = region.GetEquationNumber(eqindex, h);
            const size_t rowt = region.GetEquationNumber(eqindex, t);
            const size_t colh = region.GetEquationNumber(eqindex2, h);
            const size_t colt = region.GetEquationNumber(eqindex2, t);

            const double valh = edd0[(*it)->GetIndex()];
            const double valt = edd1[(*it)->GetIndex()];
            //// Copy Pattern from Equation::UnSymmetricEdgeAssembleJacobian
            m.push_back(dsMath::RealRowColVal(rowh, colh,  valh));
            m.push_back(dsMath::RealRowColVal(rowt, colt, -valt));
            m.push_back(dsMath::RealRowColVal(rowh, colt,  valt));
            m.push_back(dsMath::RealRowColVal(rowt, colh, -valh));
          }
        }
      }
    }

    if (ccol != size_t(-1))
    {
  //os << "ccol" <<  __LINE__ << "\n";
      std::string dermodel = emodel;
      dermodel += ":";  // should use function to create name for derivative model
      dermodel += circuitnode;

      ConstEdgeModelPtr edm = region.GetEdgeModel(dermodel);

      if (!edm)
      {
        dsErrors::MissingContactEquationModel(region, *this, dermodel, dsErrors::ModelInfo::EDGE, OutputStream::VERBOSE1);
      }
      else
      {
        EdgeScalarData edd(*ec);
        edd *= *edm;

        const Region::NodeToConstEdgeList_t &ntelist = region.GetNodeToEdgeList();

        for (ConstNodeList_t::const_iterator cit = cnodes.begin() ; cit != cnodes.end(); ++cit)
        {
          const ConstEdgeList &el = ntelist[(*cit)->GetIndex()];
          for (ConstEdgeList::const_iterator it = el.begin() ; it != el.end(); ++it)
          {
            const Node *h = (*it)->GetHead();
            const Node *t = (*it)->GetTail();
            const size_t rowh = region.GetEquationNumber(eqindex, h);
            const size_t rowt = region.GetEquationNumber(eqindex, t);

            const double val = edd[(*it)->GetIndex()];

            m.push_back(dsMath::RealRowColVal(rowh, ccol,  val));
            m.push_back(dsMath::RealRowColVal(rowt, ccol, -val));
          }
        }
      }
    }
  }
}

void ContactEquation::AssembleTriangleEdgeEquation(const std::string &emodel, dsMath::RealRowColValueVec &m, RHSEntryVec &v, dsMathEnum::WhatToLoad w, const std::string &edge_couple, const double n0_sign, const double n1_sign)
{
  typedef std::vector<std::string> VariableList_t;

  const ConstNodeList_t cnodes = GetActiveNodes();
  const Region &region = GetRegion();

  const size_t eqindex = region.GetEquationIndex(GetName());
//  dsAssert(eqindex != size_t(-1), "UNEXPECTED");
  if (eqindex == size_t(-1))
  {
    dsErrors::MissingContactEquationIndex(region, *mycontact, GetName(), OutputStream::FATAL) ;
  }

  dsAssert(!emodel.empty(), "UNEXPECTED");

  ConstTriangleEdgeModelPtr ec = region.GetTriangleEdgeModel(edge_couple);
//  dsAssert(ec != NULL, "UNEXPECTED");
  if (!ec)
  {
    dsErrors::MissingContactEquationModel(region, *this, edge_couple, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::FATAL);
  }

  size_t ccol = size_t(-1);

  if (!circuitnode.empty())
  {
//      os << "DEBC2: circuit node " << emodel << " " << GetContact().GetName() << " " << GetName() << " " << circuitnode << "\n";
    NodeKeeper &nk = NodeKeeper::instance();
    if (nk.IsCircuitNode(circuitnode))
    {
        ccol = nk.GetEquationNumber(circuitnode);
    }
    else
    {
      dsErrors::MissingCircuitNodeOnContactEquation(*this, circuitnode, OutputStream::FATAL);
    }
  }

  if ((w == dsMathEnum::RHS) || (w == dsMathEnum::MATRIXANDRHS))
  {
    ConstTriangleEdgeModelPtr em = region.GetTriangleEdgeModel(emodel);
//    dsAssert(em != NULL, "UNEXPECTED");
    if (!em)
    {
      dsErrors::MissingContactEquationModel(region, *this, emodel, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::FATAL);
    }


    TriangleEdgeScalarData esd(*ec);
    esd *= *em;

    const Region::TriangleToConstEdgeList_t &ttelist = region.GetTriangleToEdgeList();
    const Region::NodeToConstTriangleList_t &nttlist = region.GetNodeToTriangleList();

    for (ConstNodeList_t::const_iterator cit = cnodes.begin(); cit != cnodes.end(); ++cit)
    {
      const ConstTriangleList &ntl = nttlist[(*cit)->GetIndex()];
      for (ConstTriangleList::const_iterator ti = ntl.begin(); ti != ntl.end(); ++ti)
      {
        const Triangle &triangle = **ti;
        const size_t tindex = triangle.GetIndex();
        const ConstEdgeList &edgeList = ttelist[tindex];
        for (size_t eindex = 0; eindex < edgeList.size(); ++eindex)
        {
          const Edge &edge = *edgeList[eindex];
          if ((edge.GetHead() == (*cit)) || (edge.GetTail() == (*cit)))
          {
            const Node *h = edge.GetHead();
            const Node *t = edge.GetTail();
            const size_t rowh = region.GetEquationNumber(eqindex, h);
            const size_t rowt = region.GetEquationNumber(eqindex, t);

            double val = esd[3 * tindex + eindex];

            v.push_back(std::make_pair(rowh, n0_sign * val));
            v.push_back(std::make_pair(rowt, n1_sign * val));
          } 
        }
      }
    }
  }

  if ((w == dsMathEnum::MATRIXONLY) || (w == dsMathEnum::MATRIXANDRHS))
  {
    const Region::TriangleToConstEdgeList_t &ttelist = region.GetTriangleToEdgeList();
    const Region::NodeToConstTriangleList_t &nttlist = region.GetNodeToTriangleList();

    const VariableList_t &vlist = region.GetVariableList();
    for (VariableList_t::const_iterator it = vlist.begin(); it != vlist.end(); ++it)
    {
      const std::string var(*it);
      std::string dermodel = emodel;
      dermodel += ":";  // should use function to create name for derivative model
      dermodel += var;

      std::string dermodel0 = dermodel + "@en0";
      std::string dermodel1 = dermodel + "@en1";
      std::string dermodel2 = dermodel + "@en2";

      ConstTriangleEdgeModelPtr edm0 = region.GetTriangleEdgeModel(dermodel0);
      ConstTriangleEdgeModelPtr edm1 = region.GetTriangleEdgeModel(dermodel1);
      ConstTriangleEdgeModelPtr edm2 = region.GetTriangleEdgeModel(dermodel2);

      if (!edm0)
      {
        dsErrors::MissingContactEquationModel(region, *this, dermodel0, dsErrors::ModelInfo::EDGE, OutputStream::VERBOSE1);
      }
      else
      {
        TriangleEdgeScalarData edd0(*ec);
        TriangleEdgeScalarData edd1(*ec);
        TriangleEdgeScalarData edd2(*ec);
        edd0 *= *edm0;
        edd1 *= *edm1;
        edd2 *= *edm2;

        const size_t eqindex2 = region.GetEquationIndex(region.GetEquationNameFromVariable(var));
//        dsAssert(eqindex2 != size_t(-1), "UNEXPECTED");
        if (eqindex2 == size_t(-1))
        {
          dsErrors::MissingEquationIndex(region, myname, var, OutputStream::FATAL) ;
        }

        for (ConstNodeList_t::const_iterator cit = cnodes.begin(); cit != cnodes.end(); ++cit)
        {
          const ConstTriangleList &ntl = nttlist[(*cit)->GetIndex()];

          for (ConstTriangleList::const_iterator ti = ntl.begin(); ti != ntl.end(); ++ti)
          {
            const Triangle &triangle = **ti;
            const size_t tindex = triangle.GetIndex();
            const ConstNodeList &tnl = triangle.GetNodeList();
            const ConstEdgeList &edgeList = ttelist[tindex];
            for (size_t eindex = 0; eindex < edgeList.size(); ++eindex)
            {
              const Edge &edge = *edgeList[eindex];
              const Node * const h = edge.GetHead();
              const Node * const t = edge.GetTail();
              if ((h == (*cit)) || (t == (*cit)))
              {
                //// we are guaranteed that the node is across from the edge
                const Node *const o = tnl[eindex];

                const size_t rowh = region.GetEquationNumber(eqindex, h);
                const size_t rowt = region.GetEquationNumber(eqindex, t);
                const size_t colh = region.GetEquationNumber(eqindex2, h);
                const size_t colt = region.GetEquationNumber(eqindex2, t);

                const size_t colo = region.GetEquationNumber(eqindex2, o);

                const size_t vindex = 3 * tindex + eindex;

                const double valh = edd0[vindex];
                const double valt = edd1[vindex];
                const double valo = edd2[vindex];

                m.push_back(dsMath::RealRowColVal(rowh, colh, n0_sign * valh));
                m.push_back(dsMath::RealRowColVal(rowt, colt, n1_sign * valt));
                m.push_back(dsMath::RealRowColVal(rowh, colt, n0_sign * valt));
                m.push_back(dsMath::RealRowColVal(rowt, colh, n1_sign * valh));

                m.push_back(dsMath::RealRowColVal(rowh, colo, n0_sign * valo));
                m.push_back(dsMath::RealRowColVal(rowt, colo, n1_sign * valo));
              }
            }
          }
        }
      }
    }

    if (ccol != size_t(-1))
    {
//os << "ccol" <<  __LINE__ << "\n";
      std::string dermodel = emodel;
      dermodel += ":";  // should use function to create name for derivative model
      dermodel += circuitnode;

      ConstTriangleEdgeModelPtr edm = region.GetTriangleEdgeModel(dermodel);

      if (!edm)
      {
        dsErrors::MissingContactEquationModel(region, *this, dermodel, dsErrors::ModelInfo::EDGE, OutputStream::VERBOSE1);
      }
      else
      {
        TriangleEdgeScalarData edd(*ec);
        edd *= *edm;

        for (ConstNodeList_t::const_iterator cit = cnodes.begin(); cit != cnodes.end(); ++cit)
        {
          const ConstTriangleList &ntl = nttlist[(*cit)->GetIndex()];

          for (ConstTriangleList::const_iterator ti = ntl.begin(); ti != ntl.end(); ++ti)
          {
            const Triangle &triangle = **ti;
            const size_t tindex = triangle.GetIndex();
            const ConstEdgeList &edgeList = ttelist[tindex];
            for (size_t eindex = 0; eindex < edgeList.size(); ++eindex)
            {
              const Edge &edge = *edgeList[eindex];
              const Node *h = edge.GetHead();
              const Node *t = edge.GetTail();
              if ((h == (*cit)) || (t == (*cit)))
              {
                const size_t rowh = region.GetEquationNumber(eqindex, h);
                const size_t rowt = region.GetEquationNumber(eqindex, t);

                const double val = edd[3*tindex + eindex];

                m.push_back(dsMath::RealRowColVal(rowh, ccol, n0_sign * val));
                m.push_back(dsMath::RealRowColVal(rowt, ccol, n1_sign * val));
              }
            }
          }
        }
      }
    }
  }
}

// TODO:"REVIEW CODE"
void ContactEquation::AssembleTetrahedronEdgeEquation(const std::string &emodel, dsMath::RealRowColValueVec &m, RHSEntryVec &v, dsMathEnum::WhatToLoad w, const std::string &edge_couple, const double n0_sign, const double n1_sign)
{
  typedef std::vector<std::string> VariableList_t;

  const ConstNodeList_t cnodes = GetActiveNodes();
  const Region &region = GetRegion();

  const size_t eqindex = region.GetEquationIndex(GetName());
//  dsAssert(eqindex != size_t(-1), "UNEXPECTED");
  if (eqindex == size_t(-1))
  {
    dsErrors::MissingContactEquationIndex(region, *mycontact, GetName(), OutputStream::FATAL) ;
  }

  dsAssert(!emodel.empty(), "UNEXPECTED");

  ConstTetrahedronEdgeModelPtr ec = region.GetTetrahedronEdgeModel(edge_couple);
//  dsAssert(ec != NULL, "UNEXPECTED");
  if (!ec)
  {
    dsErrors::MissingContactEquationModel(region, *this, edge_couple, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::FATAL);
  }

  size_t ccol = size_t(-1);

  if (!circuitnode.empty())
  {
//      os << "DEBC2: circuit node " << emodel << " " << GetContact().GetName() << " " << GetName() << " " << circuitnode << "\n";
    NodeKeeper &nk = NodeKeeper::instance();
    if (nk.IsCircuitNode(circuitnode))
    {
        ccol = nk.GetEquationNumber(circuitnode);
    }
    else
    {
      dsErrors::MissingCircuitNodeOnContactEquation(*this, circuitnode, OutputStream::FATAL);
    }
  }

  if ((w == dsMathEnum::RHS) || (w == dsMathEnum::MATRIXANDRHS))
  {
    ConstTetrahedronEdgeModelPtr em = region.GetTetrahedronEdgeModel(emodel);
//    dsAssert(em != NULL, "UNEXPECTED");
    if (!em)
    {
      dsErrors::MissingContactEquationModel(region, *this, emodel, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::FATAL);
    }

    TetrahedronEdgeScalarData esd(*ec);
    esd *= *em;

    const Region::TetrahedronToConstEdgeDataList_t &ttelist = region.GetTetrahedronToEdgeDataList();
    const Region::NodeToConstTetrahedronList_t &nttlist = region.GetNodeToTetrahedronList();

    for (ConstNodeList_t::const_iterator cit = cnodes.begin(); cit != cnodes.end(); ++cit)
    {
      const ConstTetrahedronList &ntl = nttlist[(*cit)->GetIndex()];
      for (ConstTetrahedronList::const_iterator ti = ntl.begin(); ti != ntl.end(); ++ti)
      {
        const Tetrahedron &tetrahedron = **ti;
        const size_t tindex = tetrahedron.GetIndex();
        const ConstEdgeDataList &edgeDataList = ttelist[tindex];
        for (size_t eindex = 0; eindex < edgeDataList.size(); ++eindex)
        {
          const Edge &edge = *(edgeDataList[eindex]->edge);
          if ((edge.GetHead() == (*cit)) || (edge.GetTail() == (*cit)))
          {
            const Node *h = edge.GetHead();
            const Node *t = edge.GetTail();
            const size_t rowh = region.GetEquationNumber(eqindex, h);
            const size_t rowt = region.GetEquationNumber(eqindex, t);

            double val = esd[6 * tindex + eindex];

            v.push_back(std::make_pair(rowh, n0_sign * val));
            v.push_back(std::make_pair(rowt, n1_sign * val));
          } 
        }
      }
    }
  }

  if ((w == dsMathEnum::MATRIXONLY) || (w == dsMathEnum::MATRIXANDRHS))
  {
    const Region::TetrahedronToConstEdgeDataList_t &ttelist = region.GetTetrahedronToEdgeDataList();
    const Region::NodeToConstTetrahedronList_t &nttlist = region.GetNodeToTetrahedronList();

    const VariableList_t &vlist = region.GetVariableList();
    for (VariableList_t::const_iterator it = vlist.begin(); it != vlist.end(); ++it)
    {
      const std::string var(*it);
      std::string dermodel = emodel;
      dermodel += ":";  // should use function to create name for derivative model
      dermodel += var;

      std::string dermodel0 = dermodel + "@en0";
      std::string dermodel1 = dermodel + "@en1";
      std::string dermodel2 = dermodel + "@en2";
      std::string dermodel3 = dermodel + "@en3";

      ConstTetrahedronEdgeModelPtr edm0 = region.GetTetrahedronEdgeModel(dermodel0);
      ConstTetrahedronEdgeModelPtr edm1 = region.GetTetrahedronEdgeModel(dermodel1);
      ConstTetrahedronEdgeModelPtr edm2 = region.GetTetrahedronEdgeModel(dermodel2);
      ConstTetrahedronEdgeModelPtr edm3 = region.GetTetrahedronEdgeModel(dermodel3);

      if (!edm0)
      {
        dsErrors::MissingContactEquationModel(region, *this, dermodel0, dsErrors::ModelInfo::EDGE, OutputStream::VERBOSE1);
      }
      else
      {
        TetrahedronEdgeScalarData edd0(*ec);
        TetrahedronEdgeScalarData edd1(*ec);
        TetrahedronEdgeScalarData edd2(*ec);
        TetrahedronEdgeScalarData edd3(*ec);
        edd0 *= *edm0;
        edd1 *= *edm1;
        edd2 *= *edm2;
        edd3 *= *edm3;

        const size_t eqindex2 = region.GetEquationIndex(region.GetEquationNameFromVariable(var));
//        dsAssert(eqindex2 != size_t(-1), "UNEXPECTED");
        if (eqindex2 == size_t(-1))
        {
          dsErrors::MissingEquationIndex(region, myname, var, OutputStream::FATAL) ;
        }

        for (ConstNodeList_t::const_iterator cit = cnodes.begin(); cit != cnodes.end(); ++cit)
        {
          const ConstTetrahedronList &ntl = nttlist[(*cit)->GetIndex()];

          for (ConstTetrahedronList::const_iterator ti = ntl.begin(); ti != ntl.end(); ++ti)
          {
            const Tetrahedron &tetrahedron = **ti;
            const size_t tindex = tetrahedron.GetIndex();
//            const ConstNodeList &tnl = tetrahedron.GetNodeList();
            const ConstEdgeDataList &edgeDataList = ttelist[tindex];
            for (size_t eindex = 0; eindex < edgeDataList.size(); ++eindex)
            {
              const EdgeData &edgeData = *edgeDataList[eindex];
              const Edge &edge = *edgeData.edge;

              const Node * const h = edge.GetHead();
              const Node * const t = edge.GetTail();
              if ((h == (*cit)) || (t == (*cit)))
              {
                //// these are the nodes across the triangle face of the tetrahedron
                const Node * ot2 = edgeData.nodeopp[0];
                const Node * ot3 = edgeData.nodeopp[1];

                const size_t rowh = region.GetEquationNumber(eqindex, h);
                const size_t rowt = region.GetEquationNumber(eqindex, t);
                const size_t colh = region.GetEquationNumber(eqindex2, h);
                const size_t colt = region.GetEquationNumber(eqindex2, t);

                const size_t colo2 = region.GetEquationNumber(eqindex2, ot2);
                const size_t colo3 = region.GetEquationNumber(eqindex2, ot3);

                const size_t vindex = 6 * tindex + eindex;

                const double valh = edd0[vindex];
                const double valt = edd1[vindex];
                const double valo2 = edd2[vindex];
                const double valo3 = edd3[vindex];

                m.push_back(dsMath::RealRowColVal(rowh, colh, n0_sign * valh));
                m.push_back(dsMath::RealRowColVal(rowt, colt, n1_sign * valt));
                m.push_back(dsMath::RealRowColVal(rowh, colt, n0_sign * valt));
                m.push_back(dsMath::RealRowColVal(rowt, colh, n1_sign * valh));

                m.push_back(dsMath::RealRowColVal(rowh, colo2, n0_sign * valo2));
                m.push_back(dsMath::RealRowColVal(rowt, colo2, n1_sign * valo2));
                m.push_back(dsMath::RealRowColVal(rowh, colo3, n0_sign * valo3));
                m.push_back(dsMath::RealRowColVal(rowt, colo3, n1_sign * valo3));
              }
            }
          }
        }
      }
    }

    if (ccol != size_t(-1))
    {
//os << "ccol" <<  __LINE__ << "\n";
      std::string dermodel = emodel;
      dermodel += ":";  // should use function to create name for derivative model
      dermodel += circuitnode;

      ConstTetrahedronEdgeModelPtr edm = region.GetTetrahedronEdgeModel(dermodel);

      if (!edm)
      {
        dsErrors::MissingContactEquationModel(region, *this, dermodel, dsErrors::ModelInfo::EDGE, OutputStream::VERBOSE1);
      }
      else
      {
        TetrahedronEdgeScalarData edd(*ec);
        edd *= *edm;

        for (ConstNodeList_t::const_iterator cit = cnodes.begin(); cit != cnodes.end(); ++cit)
        {
          const ConstTetrahedronList &ntl = nttlist[(*cit)->GetIndex()];

          for (ConstTetrahedronList::const_iterator ti = ntl.begin(); ti != ntl.end(); ++ti)
          {
            const Tetrahedron &tetrahedron = **ti;
            const size_t tindex = tetrahedron.GetIndex();
            const ConstEdgeDataList &edgeDataList = ttelist[tindex];
            for (size_t eindex = 0; eindex < edgeDataList.size(); ++eindex)
            {
              const Edge &edge = *(edgeDataList[eindex]->edge);
              const Node *h = edge.GetHead();
              const Node *t = edge.GetTail();
              if ((h == (*cit)) || (t == (*cit)))
              {
                const size_t rowh = region.GetEquationNumber(eqindex, h);
                const size_t rowt = region.GetEquationNumber(eqindex, t);

                const double val = edd[6*tindex + eindex];

                m.push_back(dsMath::RealRowColVal(rowh, ccol, n0_sign * val));
                m.push_back(dsMath::RealRowColVal(rowt, ccol, n1_sign * val));
              }
            }
          }
        }
      }
    }
  }
}

void ContactEquation::AssembleElementEdgeEquation(const std::string &emodel, dsMath::RealRowColValueVec &m, RHSEntryVec &v, dsMathEnum::WhatToLoad w, const std::string &edge_couple, const double n0_sign, const double n1_sign)
{
  const size_t dimension = GetRegion().GetDimension();
  if (dimension == 2)
  {
    AssembleTriangleEdgeEquation(emodel, m, v, w, edge_couple, n0_sign, n1_sign);
  }
  else if (dimension == 3)
  {
    AssembleTetrahedronEdgeEquation(emodel, m, v, w, edge_couple, n0_sign, n1_sign);
  }
}

void ContactEquation::AssembleEdgeEquationOnCircuit(const std::string &emodel, dsMath::RealRowColValueVec &m, RHSEntryVec &v, dsMathEnum::WhatToLoad w, const std::string &edge_couple)
{
  dsAssert(!emodel.empty(), "UNEXPECTED");
  dsAssert(!circuitnode.empty(), "UNEXPECTED");

  const ConstNodeList_t cnodes = GetActiveNodes();
  const Region &region = GetRegion();

  size_t crow = size_t(-1);
  NodeKeeper &nk = NodeKeeper::instance();
  if (nk.IsCircuitNode(circuitnode))
  {
    crow = nk.GetEquationNumber(circuitnode);
  }
  else
  {
    dsErrors::MissingCircuitNodeOnContactEquation(*this, circuitnode, OutputStream::FATAL);
  }

  //////
  //////  End execution here
  //////
  if (crow == size_t(-1))
  {
    return;
  }

  ConstEdgeModelPtr ec = region.GetEdgeModel(edge_couple);
//  dsAssert(ec != NULL, "UNEXPECTED");
  if (!ec)
  {
    dsErrors::MissingContactEquationModel(region, *this, edge_couple, dsErrors::ModelInfo::EDGE, OutputStream::FATAL);
  }

  if ((w == dsMathEnum::RHS) || (w == dsMathEnum::MATRIXANDRHS))
  {
    ConstEdgeModelPtr em = region.GetEdgeModel(emodel);
//    dsAssert(em != NULL, "UNEXPECTED");
    if (!em)
    {
      dsErrors::MissingContactEquationModel(region, *this, emodel, dsErrors::ModelInfo::EDGE, OutputStream::FATAL);
    }

    EdgeScalarData esd(*ec);
    esd *= *em;

    const Region::NodeToConstEdgeList_t &ntelist = region.GetNodeToEdgeList();

    for (ConstNodeList_t::const_iterator cit = cnodes.begin(); cit != cnodes.end(); ++cit)
    {
      double rhsval = 0.0;

      const ConstEdgeList &el = ntelist[(*cit)->GetIndex()];
      for (ConstEdgeList::const_iterator it = el.begin() ; it != el.end(); ++it)
      {
        if (bothNodesOnContact(cnodes, **it))
        {
          continue;
        }

        double val = (*it)->GetNodeSign((*cit));
        val *= esd[(*it)->GetIndex()];
        rhsval += val;
      }

      v.push_back(std::make_pair(crow,  rhsval));
    }
  }


  if ((w == dsMathEnum::MATRIXONLY) || (w == dsMathEnum::MATRIXANDRHS))
  {
    const VariableList_t &vlist = region.GetVariableList();
    for (VariableList_t::const_iterator it = vlist.begin(); it != vlist.end(); ++it)
    {
      const std::string var(*it);
      std::string dermodel = emodel;
      dermodel += ":";  // should use function to create name for derivative model
      dermodel += var;

      std::string dermodel0 = dermodel + "@n0";
      std::string dermodel1 = dermodel + "@n1";

      ConstEdgeModelPtr edm0 = region.GetEdgeModel(dermodel0);
      ConstEdgeModelPtr edm1 = region.GetEdgeModel(dermodel1);

      if (!edm0)
      {
        dsErrors::MissingContactEquationModel(region, *this, dermodel0, dsErrors::ModelInfo::EDGE, OutputStream::VERBOSE1);
      }
      else
      {
//          os << "Contact assemble: using model " << dermodel0 << "\n";
        EdgeScalarData edd0(*ec);
        EdgeScalarData edd1(*ec);
        edd0 *= *edm0;
        edd1 *= *edm1;

        const size_t eqindex2 = region.GetEquationIndex(region.GetEquationNameFromVariable(var));
//        dsAssert(eqindex2 != size_t(-1), "UNEXPECTED");
        if (eqindex2 == size_t(-1))
        {
          dsErrors::MissingEquationIndex(region, myname, var, OutputStream::FATAL) ;
        }

        const Region::NodeToConstEdgeList_t &ntelist = region.GetNodeToEdgeList();

        for (ConstNodeList_t::const_iterator cit = cnodes.begin(); cit != cnodes.end(); ++cit)
        {
          const ConstEdgeList &el = ntelist[(*cit)->GetIndex()];

          const size_t col = region.GetEquationNumber(eqindex2, (*cit));

          double val = 0.0;
          for (ConstEdgeList::const_iterator it = el.begin() ; it != el.end(); ++it)
          {
            if (bothNodesOnContact(cnodes, **it))
            {
              continue;
            }

            double val2 = 0.0;
            size_t col2 = 0;

//          const double ns = (*it)->GetNodeSign((*cit));
            const  size_t eind = (*it)->GetIndex();

            //// If our contact node is on node 0 or node 1 of the edge
            //// We need to get the right sign and derivative
            //// for unsymmetric derivatives
            //// Maintain the sign from above
            const Node *h = (*it)->GetHead();
            const Node *t = (*it)->GetTail();

            if ((*cit) == h)
            {
              val += edd0[eind];

              val2 = edd1[eind];
              col2 = region.GetEquationNumber(eqindex2, t);
            }
            else if ((*cit) == t)
            {
              val -=  edd1[eind];

              val2 = -edd0[eind];
              col2 = region.GetEquationNumber(eqindex2, h);
            }
            else
            {
              dsAssert(0, "UNEXPECTED");
            }

            m.push_back(dsMath::RealRowColVal(crow, col2, val2));
//              os << "e2 " << crow << " " << col2 << " " << val2 << "\n";
          }

          m.push_back(dsMath::RealRowColVal(crow, col, val));
//          os << "e1 " << crow << " " << col << " " << val << "\n";
        }
      }

      {
        std::string dermodel = emodel;
        dermodel += ":" + circuitnode;
        ConstEdgeModelPtr edm = region.GetEdgeModel(dermodel);
        if (!edm)
        {
          dsErrors::MissingContactEquationModel(region, *this, dermodel, dsErrors::ModelInfo::EDGE, OutputStream::VERBOSE1);
        }
        else
        {
//              os << "Contact assemble: using model " << dermodel << "\n";
          EdgeScalarData edd(*ec);
          edd *= *edm;

          const Region::NodeToConstEdgeList_t &ntelist = region.GetNodeToEdgeList();

          for (ConstNodeList_t::const_iterator cit = cnodes.begin(); cit != cnodes.end(); ++cit)
          {
            const ConstEdgeList &el = ntelist[(*cit)->GetIndex()];

            for (ConstEdgeList::const_iterator it = el.begin() ; it != el.end(); ++it)
            {
              if (bothNodesOnContact(cnodes, **it))
              {
                continue;
              }

              const  size_t eind = (*it)->GetIndex();
              const double val = edd[eind];

              if ((*it)->GetHead() == (*cit))
              {
                m.push_back(dsMath::RealRowColVal(crow, crow, val));
//                  os << "jjj " <<  crow << " " << crow << val << "\n";
              }
              else if ((*it)->GetTail() == (*cit))
              {
                m.push_back(dsMath::RealRowColVal(crow, crow,-val));
//                  os << "jjj " <<  crow << " " << crow << -val << "\n";
              }
              else
              {
                dsAssert(0, "UNEXPECTED");
              }
            }
          }
        }
      }
    }
  }
}

void ContactEquation::AssembleTriangleEdgeEquationOnCircuit(const std::string &emodel, dsMath::RealRowColValueVec &m, RHSEntryVec &v, dsMathEnum::WhatToLoad w, const std::string &edge_couple, const double n0_sign, const double n1_sign)
{
  dsAssert(!emodel.empty(), "UNEXPECTED");
  dsAssert(!circuitnode.empty(), "UNEXPECTED");

  const ConstNodeList_t cnodes = GetActiveNodes();
  const Region &region = GetRegion();

  size_t crow = size_t(-1);
  NodeKeeper &nk = NodeKeeper::instance();
  if (nk.IsCircuitNode(circuitnode))
  {
    crow = nk.GetEquationNumber(circuitnode);
  }
  else
  {
      dsErrors::MissingCircuitNodeOnContactEquation(*this, circuitnode, OutputStream::FATAL);
  }

  //////
  //////  End execution here
  //////
  if (crow == size_t(-1))
  {
    return;
  }


  ConstTriangleEdgeModelPtr ec = region.GetTriangleEdgeModel(edge_couple);
//  dsAssert(ec != NULL, "UNEXPECTED");
  if (!ec)
  {
    dsErrors::MissingContactEquationModel(region, *this, edge_couple, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::FATAL);
  }

  if ((w == dsMathEnum::RHS) || (w == dsMathEnum::MATRIXANDRHS))
  {
    ConstTriangleEdgeModelPtr em = region.GetTriangleEdgeModel(emodel);
//    dsAssert(em != NULL, "UNEXPECTED");
    if (!em)
    {
      dsErrors::MissingContactEquationModel(region, *this, emodel, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::FATAL);
    }


    TriangleEdgeScalarData esd(*ec);
    esd *= *em;

    const Region::TriangleToConstEdgeList_t &ttelist = region.GetTriangleToEdgeList();
    const Region::NodeToConstTriangleList_t &nttlist = region.GetNodeToTriangleList();

    for (ConstNodeList_t::const_iterator cit = cnodes.begin(); cit != cnodes.end(); ++cit)
    {
      const ConstTriangleList &ntl = nttlist[(*cit)->GetIndex()];
      for (ConstTriangleList::const_iterator ti = ntl.begin(); ti != ntl.end(); ++ti)
      {
        const Triangle &triangle = **ti;
        const size_t tindex = triangle.GetIndex();
        const ConstEdgeList &edgeList = ttelist[tindex];

        double rhsval = 0.0;

        for (size_t eindex = 0; eindex < edgeList.size(); ++eindex)
        {
          const Edge &edge = *edgeList[eindex];
          if ((edge.GetHead() == (*cit)) || (edge.GetTail() == (*cit)))
          {
            if (bothNodesOnContact(cnodes, edge))
            {
              continue;
            }

            
            double val;
            if (*cit == edge.GetHead())
            {
              val = n0_sign;
            }
            else
            {
              val = n1_sign;
            }

            val *= esd[3 * tindex + eindex];
            rhsval += val;

            v.push_back(std::make_pair(crow,  rhsval));
          }
        }
      }
    }


    if ((w == dsMathEnum::MATRIXONLY) || (w == dsMathEnum::MATRIXANDRHS))
    {
      const Region::TriangleToConstEdgeList_t &ttelist = region.GetTriangleToEdgeList();
      const Region::NodeToConstTriangleList_t &nttlist = region.GetNodeToTriangleList();

      for (ConstNodeList_t::const_iterator cit = cnodes.begin(); cit != cnodes.end(); ++cit)
      {
        const VariableList_t &vlist = region.GetVariableList();

        for (VariableList_t::const_iterator it = vlist.begin(); it != vlist.end(); ++it)
        {
          const std::string var(*it);
          std::string dermodel = emodel;
          dermodel += ":";  // should use function to create name for derivative model
          dermodel += var;

          std::string dermodel0 = dermodel + "@en0";
          std::string dermodel1 = dermodel + "@en1";
          std::string dermodel2 = dermodel + "@en2";

          ConstTriangleEdgeModelPtr edm0 = region.GetTriangleEdgeModel(dermodel0);
          ConstTriangleEdgeModelPtr edm1 = region.GetTriangleEdgeModel(dermodel1);
          ConstTriangleEdgeModelPtr edm2 = region.GetTriangleEdgeModel(dermodel2);

          if (!edm0)
          {
            dsErrors::MissingContactEquationModel(region, *this, dermodel0, dsErrors::ModelInfo::EDGE, OutputStream::VERBOSE1);
          }
          else
          {
//              os << "Contact assemble: using model " << dermodel0 << "\n";
            TriangleEdgeScalarData edd0(*ec);
            TriangleEdgeScalarData edd1(*ec);
            TriangleEdgeScalarData edd2(*ec);
            edd0 *= *edm0;
            edd1 *= *edm1;
            edd2 *= *edm2;

            const size_t eqindex2 = region.GetEquationIndex(region.GetEquationNameFromVariable(var));
//            dsAssert(eqindex2 != size_t(-1), "UNEXPECTED");
            if (eqindex2 == size_t(-1))
            {
              dsErrors::MissingEquationIndex(region, myname, var, OutputStream::FATAL) ;
            }

            const size_t col = region.GetEquationNumber(eqindex2, (*cit));

            double val = 0.0;

            const ConstTriangleList &ntl = nttlist[(*cit)->GetIndex()];
            for (ConstTriangleList::const_iterator ti = ntl.begin(); ti != ntl.end(); ++ti)
            {
              const Triangle &triangle = **ti;
              const size_t tindex = triangle.GetIndex();
              const ConstEdgeList &edgeList = ttelist[tindex];
              const ConstNodeList &tnl = triangle.GetNodeList();
              for (size_t eindex = 0; eindex < edgeList.size(); ++eindex)
              {
                const Edge &edge = *edgeList[eindex];
                if ((edge.GetHead() == (*cit)) || (edge.GetTail() == (*cit)))
                {
                  if (bothNodesOnContact(cnodes, edge))
                  {
                    continue;
                  }

                  const size_t vindex = 3 * tindex + eindex;

                  const Node *h = edge.GetHead();
                  const Node *t = edge.GetTail();

                  double val1 = 0.0;
                  size_t col1 = 0;

                  double val2 = 0.0;
                  size_t col2 = 0;

                  //// we are guaranteed that the node is across from the edge
                  const Node *const o = tnl[eindex];

                  //// If our contact node is on node 0 or node 1 of the edge
                  //// We need to get the right sign and derivative
                  //// for unsymmetric derivatives
                  //// Maintain the sign from above
                  if ((*cit) == h)
                  {
                    val += n0_sign * edd0[vindex];

                    val1 = n0_sign * edd1[vindex];
                    col1 = region.GetEquationNumber(eqindex2, t);

                    val2 = n0_sign * edd2[vindex];
                    col2 = region.GetEquationNumber(eqindex2, o);
                  }
                  else if ((*cit) == t)
                  {
                    val += n1_sign * edd1[vindex];

                    val1 = n1_sign * edd0[vindex];
                    col1 = region.GetEquationNumber(eqindex2, h);

                    val2 = n1_sign * edd2[vindex];
                    col2 = region.GetEquationNumber(eqindex2, o);
                  }
                  else
                  {
                    dsAssert(0, "UNEXPECTED");
                  }
                  m.push_back(dsMath::RealRowColVal(crow, col1, val1));
                  m.push_back(dsMath::RealRowColVal(crow, col2, val2));
                }
                m.push_back(dsMath::RealRowColVal(crow, col, val));
    //          os << "e1 " << crow << " " << col << " " << val << "\n";
              }
            }
          }
        }

        {
          std::string dermodel = emodel;
          dermodel += ":" + circuitnode;
          ConstTriangleEdgeModelPtr edm = region.GetTriangleEdgeModel(dermodel);
          if (!edm)
          {
            dsErrors::MissingContactEquationModel(region, *this, dermodel, dsErrors::ModelInfo::EDGE, OutputStream::VERBOSE1);
          }
          else
          {
//                  os << "Contact assemble: using model " << dermodel << "\n";
            TriangleEdgeScalarData edd(*ec);
            edd *= *edm;

            const ConstTriangleList &ntl = nttlist[(*cit)->GetIndex()];
            for (ConstTriangleList::const_iterator ti = ntl.begin(); ti != ntl.end(); ++ti)
            {
              const Triangle &triangle = **ti;
              const size_t tindex = triangle.GetIndex();
              const ConstEdgeList &edgeList = ttelist[tindex];
              for (size_t eindex = 0; eindex < edgeList.size(); ++eindex)
              {
                const Edge &edge = *edgeList[eindex];
                if ((edge.GetHead() == (*cit)) || (edge.GetTail() == (*cit)))
                {
                  if (bothNodesOnContact(cnodes, edge))
                  {
                    continue;
                  }

                  const size_t vindex = 3 * tindex + eindex;

                  const Node *h = edge.GetHead();
                  const Node *t = edge.GetTail();

                  const double val = edd[vindex];

                  if (h == (*cit))
                  {
                    m.push_back(dsMath::RealRowColVal(crow, crow, n0_sign * val));
  //                  os << "jjj " <<  crow << " " << crow << val << "\n";
                  }
                  else if (t == (*cit))
                  {
                    m.push_back(dsMath::RealRowColVal(crow, crow, n1_sign * val));
  //                  os << "jjj " <<  crow << " " << crow << -val << "\n";
                  }
                  else
                  {
                      dsAssert(0, "UNEXPECTED");
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

// TODO:"REVIEW CODE"
void ContactEquation::AssembleTetrahedronEdgeEquationOnCircuit(const std::string &emodel, dsMath::RealRowColValueVec &m, RHSEntryVec &v, dsMathEnum::WhatToLoad w, const std::string &edge_couple, const double n0_sign, const double n1_sign)
{
  dsAssert(!emodel.empty(), "UNEXPECTED");

  dsAssert(!circuitnode.empty(), "UNEXPECTED");

  const ConstNodeList_t cnodes = GetActiveNodes();
  const Region &region = GetRegion();

  size_t crow = size_t(-1);
  NodeKeeper &nk = NodeKeeper::instance();
  if (nk.IsCircuitNode(circuitnode))
  {
    crow = nk.GetEquationNumber(circuitnode);
  }
  else
  {
      dsErrors::MissingCircuitNodeOnContactEquation(*this, circuitnode, OutputStream::FATAL);
  }

  //////
  //////  End execution here
  //////
  if (crow == size_t(-1))
  {
    return;
  }


  ConstTetrahedronEdgeModelPtr ec = region.GetTetrahedronEdgeModel(edge_couple);
//  dsAssert(ec != NULL, "UNEXPECTED");
  if (!ec)
  {
    dsErrors::MissingContactEquationModel(region, *this, edge_couple, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::FATAL);
  }

  if ((w == dsMathEnum::RHS) || (w == dsMathEnum::MATRIXANDRHS))
  {
    ConstTetrahedronEdgeModelPtr em = region.GetTetrahedronEdgeModel(emodel);
//    dsAssert(em != NULL, "UNEXPECTED");
    if (!em)
    {
      dsErrors::MissingContactEquationModel(region, *this, emodel, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::FATAL);
    }

    TetrahedronEdgeScalarData esd(*ec);
    esd *= *em;

    const Region::TetrahedronToConstEdgeDataList_t &ttelist = region.GetTetrahedronToEdgeDataList();
    const Region::NodeToConstTetrahedronList_t &nttlist = region.GetNodeToTetrahedronList();

    for (ConstNodeList_t::const_iterator cit = cnodes.begin(); cit != cnodes.end(); ++cit)
    {
      const ConstTetrahedronList &ntl = nttlist[(*cit)->GetIndex()];
      for (ConstTetrahedronList::const_iterator ti = ntl.begin(); ti != ntl.end(); ++ti)
      {
        const Tetrahedron &tetrahedron = **ti;
        const size_t tindex = tetrahedron.GetIndex();
        const ConstEdgeDataList &edgeDataList = ttelist[tindex];

        double rhsval = 0.0;

        for (size_t eindex = 0; eindex < edgeDataList.size(); ++eindex)
        {
          const Edge &edge = *(edgeDataList[eindex]->edge);
          if ((edge.GetHead() == (*cit)) || (edge.GetTail() == (*cit)))
          {
            if (bothNodesOnContact(cnodes, edge))
            {
              continue;
            }

            double val;
            if (*cit == edge.GetHead())
            {
              val = n0_sign;
            }
            else
            {
              val = n1_sign;
            }

            val *= esd[6 * tindex + eindex];
            rhsval += val;

            v.push_back(std::make_pair(crow,  rhsval));
          }
        }
      }
    }


    if ((w == dsMathEnum::MATRIXONLY) || (w == dsMathEnum::MATRIXANDRHS))
    {
      const Region::TetrahedronToConstEdgeDataList_t &ttelist = region.GetTetrahedronToEdgeDataList();
      const Region::NodeToConstTetrahedronList_t &nttlist = region.GetNodeToTetrahedronList();

      for (ConstNodeList_t::const_iterator cit = cnodes.begin(); cit != cnodes.end(); ++cit)
      {
        const VariableList_t &vlist = region.GetVariableList();

        for (VariableList_t::const_iterator it = vlist.begin(); it != vlist.end(); ++it)
        {
          const std::string var(*it);
          std::string dermodel = emodel;
          dermodel += ":";  // should use function to create name for derivative model
          dermodel += var;

          std::string dermodel0 = dermodel + "@en0";
          std::string dermodel1 = dermodel + "@en1";
          std::string dermodel2 = dermodel + "@en2";
          std::string dermodel3 = dermodel + "@en3";

          ConstTetrahedronEdgeModelPtr edm0 = region.GetTetrahedronEdgeModel(dermodel0);
          ConstTetrahedronEdgeModelPtr edm1 = region.GetTetrahedronEdgeModel(dermodel1);
          ConstTetrahedronEdgeModelPtr edm2 = region.GetTetrahedronEdgeModel(dermodel2);
          ConstTetrahedronEdgeModelPtr edm3 = region.GetTetrahedronEdgeModel(dermodel3);

          if (!edm0)
          {
            dsErrors::MissingContactEquationModel(region, *this, dermodel0, dsErrors::ModelInfo::EDGE, OutputStream::VERBOSE1);
          }
          else
          {
//              os << "Contact assemble: using model " << dermodel0 << "\n";
            TetrahedronEdgeScalarData edd0(*ec);
            TetrahedronEdgeScalarData edd1(*ec);
            TetrahedronEdgeScalarData edd2(*ec);
            TetrahedronEdgeScalarData edd3(*ec);
            edd0 *= *edm0;
            edd1 *= *edm1;
            edd2 *= *edm2;
            edd3 *= *edm3;

            const size_t eqindex2 = region.GetEquationIndex(region.GetEquationNameFromVariable(var));
//            dsAssert(eqindex2 != size_t(-1), "UNEXPECTED");
            if (eqindex2 == size_t(-1))
            {
              dsErrors::MissingEquationIndex(region, myname, var, OutputStream::FATAL) ;
            }

            const size_t col = region.GetEquationNumber(eqindex2, (*cit));

            double val = 0.0;

            const ConstTetrahedronList &ntl = nttlist[(*cit)->GetIndex()];
            for (ConstTetrahedronList::const_iterator ti = ntl.begin(); ti != ntl.end(); ++ti)
            {
              const Tetrahedron &tetrahedron = **ti;
              const size_t tindex = tetrahedron.GetIndex();
              const ConstEdgeDataList &edgeDataList = ttelist[tindex];

              for (size_t eindex = 0; eindex < edgeDataList.size(); ++eindex)
              {
                const EdgeData &edgeData = *edgeDataList[eindex];
                const Edge &edge = *edgeData.edge;

                if ((edge.GetHead() == (*cit)) || (edge.GetTail() == (*cit)))
                {
                  if (bothNodesOnContact(cnodes, edge))
                  {
                    continue;
                  }

                  const size_t vindex = 6 * tindex + eindex;

                  const Node *h = edge.GetHead();
                  const Node *t = edge.GetTail();

                  double val1 = 0.0;
                  size_t col1 = 0;

                  double val2 = 0.0;
                  size_t col2 = 0;

                  double val3 = 0.0;
                  size_t col3 = 0;

                  const Node * const ot2  = edgeData.nodeopp[0];
                  const Node * const ot3  = edgeData.nodeopp[1];

                  dsAssert(ot2 != 0, "UNEXPECTED");
                  dsAssert(ot3 != 0, "UNEXPECTED");

                  //// If our contact node is on node 0 or node 1 of the edge
                  //// We need to get the right sign and derivative
                  //// for unsymmetric derivatives
                  //// Maintain the sign from above
                  if ((*cit) == h)
                  {
                    val += n0_sign * edd0[vindex];

                    val1 = n0_sign * edd1[vindex];
                    col1 = region.GetEquationNumber(eqindex2, t);

                    val2 = n0_sign * edd2[vindex];
                    col2 = region.GetEquationNumber(eqindex2, ot2);

                    val3 = n0_sign * edd3[vindex];
                    col3 = region.GetEquationNumber(eqindex2, ot3);
                  }
                  else if ((*cit) == t)
                  {
                    val += n1_sign * edd1[vindex];

                    val1 = n1_sign * edd0[vindex];
                    col1 = region.GetEquationNumber(eqindex2, h);

                    val2 = n1_sign * edd2[vindex];
                    col2 = region.GetEquationNumber(eqindex2, ot2);

                    val3 = n1_sign * edd3[vindex];
                    col3 = region.GetEquationNumber(eqindex2, ot3);
                  }
                  else
                  {
                    dsAssert(0, "UNEXPECTED");
                  }
                  m.push_back(dsMath::RealRowColVal(crow, col1, val1));
                  m.push_back(dsMath::RealRowColVal(crow, col2, val2));
                  m.push_back(dsMath::RealRowColVal(crow, col3, val3));
                }
                m.push_back(dsMath::RealRowColVal(crow, col, val));
    //          os << "e1 " << crow << " " << col << " " << val << "\n";
              }
            }
          }
        }

        {
          std::string dermodel = emodel;
          dermodel += ":" + circuitnode;
          ConstTetrahedronEdgeModelPtr edm = region.GetTetrahedronEdgeModel(dermodel);
          if (!edm)
          {
            dsErrors::MissingContactEquationModel(region, *this, dermodel, dsErrors::ModelInfo::EDGE, OutputStream::VERBOSE1);
          }
          else
          {
//                  os << "Contact assemble: using model " << dermodel << "\n";
            TetrahedronEdgeScalarData edd(*ec);
            edd *= *edm;

            const ConstTetrahedronList &ntl = nttlist[(*cit)->GetIndex()];
            for (ConstTetrahedronList::const_iterator ti = ntl.begin(); ti != ntl.end(); ++ti)
            {
              const Tetrahedron &tetrahedron = **ti;
              const size_t tindex = tetrahedron.GetIndex();
              const ConstEdgeDataList &edgeDataList = ttelist[tindex];
              for (size_t eindex = 0; eindex < edgeDataList.size(); ++eindex)
              {
                const Edge &edge = *(edgeDataList[eindex]->edge);
                if ((edge.GetHead() == (*cit)) || (edge.GetTail() == (*cit)))
                {
                  if (bothNodesOnContact(cnodes, edge))
                  {
                    continue;
                  }

                  const size_t vindex = 6 * tindex + eindex;

                  const Node *h = edge.GetHead();
                  const Node *t = edge.GetTail();

                  const double val = edd[vindex];

                  if (h == (*cit))
                  {
                    m.push_back(dsMath::RealRowColVal(crow, crow, n0_sign * val));
                  }
                  else if (t == (*cit))
                  {
                    m.push_back(dsMath::RealRowColVal(crow, crow, n1_sign * val));
                  }
                  else
                  {
                      dsAssert(0, "UNEXPECTED");
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

void ContactEquation::AssembleElementEdgeEquationOnCircuit(const std::string &emodel, dsMath::RealRowColValueVec &m, RHSEntryVec &v, dsMathEnum::WhatToLoad w, const std::string &edge_couple, const double n0_sign, const double n1_sign)
{
  const size_t dimension = GetRegion().GetDimension();
  if (dimension == 2)
  {
    AssembleTriangleEdgeEquationOnCircuit(emodel, m, v, w, edge_couple, n0_sign, n1_sign);
  }
  else if (dimension == 3)
  {
    AssembleTetrahedronEdgeEquationOnCircuit(emodel, m, v, w, edge_couple, n0_sign, n1_sign);
  }
}

void ContactEquation::DevsimSerialize(std::ostream &of) const
{
  of << "begin_equation \"" << this->GetName() << "\"\n";
  this->Serialize(of);
  of << "\nend_equation\n\n";
}

void ContactEquation::GetCommandOptions(std::map<std::string, ObjectHolder> &omap) const
{
  this->GetCommandOptions_Impl(omap);
}

