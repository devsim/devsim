/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "InterfaceEquation.hh"
#include "InterfaceEquationHolder.hh"
#include "Interface.hh"
#include "Region.hh"
#include "Device.hh"
#include "Node.hh"
#include "Contact.hh"
#include "NodeModel.hh"
#include "InterfaceNodeModel.hh"
#include "Permutation.hh"
#include "MatrixEntries.hh"
#include "GeometryStream.hh"
#include "ContactEquationHolder.hh"
#include "dsAssert.hh"
#include "EquationErrors.hh"
#include <string>
#include <vector>
#include <set>

template <typename DoubleType>
InterfaceEquation<DoubleType>::InterfaceEquation(const std::string &nm, const std::string &nm0, const std::string &nm1, InterfacePtr ip)
    : myname(nm), myname0(nm0), myname1(nm1), myinterface(ip)
{
    if (myname0.empty())
    {
      myname0 = myname;
    }
    if (myname1.empty())
    {
      myname1 = myname;
    }
    InterfaceEquationHolder tmp(this);
    ip->AddInterfaceEquation(tmp);
}

namespace {
//
// if a contact has the same equation mapping, and shares a coincident node with this interface, it is removed
//
ConstNodeList_t RemoveContactNodesFromList(const Region &region, const ConstNodeList_t &inodes, const std::string &eqname)
{
  ConstNodeList_t ret;

  const Device &device = *region.GetDevice();

  const auto &ctc = device.GetCoordinateIndexToContact();

  // check all interface nodes for presence of a contact with the same equation at the node
  for (auto &node : inodes)
  {

    // get coordinate index from node
    const size_t ci = node->GetCoordinate().GetIndex();

    // find all contacts at this coordinate and check to see if they solve this equation
    auto cit = ctc.find(ci);

    // There are no contacts on this interface node
    if (cit == ctc.end())
    {
      ret.push_back(node);
      continue;
    }

    bool active_node = true;

    for ( auto &contact : cit->second )
    {
      const Region &cregion = *(contact->GetRegion());

      if (!(cregion == region))
      {
        continue;
      }

      for ( auto &equation_map : contact->GetEquationPtrList())
      {
        if ((equation_map.second).GetName() == eqname)
        {
          active_node = false;
          break;
        }
      }

      if (!active_node)
      {
        break;
      }
    }

    if (active_node)
    {
      ret.push_back(node);
    }
  }

  return ret;
}

//
// If two interfaces with the same equation mapping share a coincident node, that node is removed
//
ConstNodeList_t RemoveInterfaceNodesFromList(const Interface &interface, const Region &region, const ConstNodeList_t &inodes, const std::string &eqname)
{
  ConstNodeList_t ret;

  const Device &device = *region.GetDevice();

  const Device::CoordinateIndexToInterface_t &cti = device.GetCoordinateIndexToInterface();

  for (auto &node : inodes)
  {
    const size_t ci = node->GetCoordinate().GetIndex();

    auto iit = cti.find(ci);

    // I think this test is impossible, since this interface should be on the coordinate
    if (iit == cti.end())
    {
      ret.push_back(node);
      continue;
    }

    bool active_node = true;

    for ( auto &ointerface : iit->second)
    {
      if (*ointerface == interface)
      {
        continue;
      }

      if (region == *(ointerface->GetRegion0()))
      {
        for (auto &equation_map : ointerface->GetInterfaceEquationList())
        {
          if ((equation_map.second).GetName0() == eqname)
          {
            active_node = false;
            break;
          }
        }
      }
      else if (region == *(ointerface->GetRegion1()))
      {
        for (auto &equation_map : ointerface->GetInterfaceEquationList())
        {
          if ((equation_map.second).GetName1() == eqname)
          {
            active_node = false;
            break;
          }
        }
      }
      if (!active_node)
      {
        break;
      }
    }

    if (active_node)
    {
      ret.push_back(node);
    }
  }

  return ret;
}
}

///// If a contact for this equation on this region already exists, we
///// will skip this node
///// If an interface for this equation on this region already exists, we skip out on that
template <typename DoubleType>
ConstNodeList_t InterfaceEquation<DoubleType>::GetActiveNodesFromList(const Region &region, const ConstNodeList_t &inputnodes) const
{

  const Interface &interface = GetInterface();


  std::string eqname;

  if (region == *(interface.GetRegion0()))
  {
    eqname = GetName0();
  }
  else if (region == *(interface.GetRegion1()))
  {
    eqname = GetName1();
  }

  const auto &cnodes = RemoveContactNodesFromList(region, inputnodes, eqname);

  const auto &ret = RemoveInterfaceNodesFromList(interface, region, cnodes, eqname);

  return ret;
}

template <typename DoubleType>
std::set<ConstNodePtr> InterfaceEquation<DoubleType>::GetActiveNodes0() const
{
  const auto &inodes0 = GetInterface().GetNodes0();
  const auto &onodes0 = GetActiveNodesFromList(*GetInterface().GetRegion0(), inodes0);

  std::set<ConstNodePtr> ret;
  for (auto onode : onodes0)
  {
    ret.insert(onode);
  }
  return ret;
}

template <typename DoubleType>
std::set<ConstNodePtr> InterfaceEquation<DoubleType>::GetActiveNodes1() const
{
  const auto &inodes1 = GetInterface().GetNodes1();
  const auto &onodes1 = GetActiveNodesFromList(*GetInterface().GetRegion1(), inodes1);

  std::set<ConstNodePtr> ret;
  for (auto onode : onodes1)
  {
    ret.insert(onode);
  }
  return ret;
}



// Should make permutation collection a separate step
template <typename DoubleType>
void InterfaceEquation<DoubleType>::Assemble(dsMath::RealRowColValueVec<DoubleType> &m, dsMath::RHSEntryVec<DoubleType> &v, PermutationMap &p, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
    DerivedAssemble(m, v, p, w, t);
}

template <typename DoubleType>
InterfaceEquation<DoubleType>::~InterfaceEquation()
{
}

namespace {
struct vlistdata {
    vlistdata(const std::string &m, const std::string &v, const Region &r, const ConstNodeList &nl) : interfacemodelname(m), var(v), region(&r), nodelist(&nl) {}

    std::string    interfacemodelname;
    std::string    var;
    const Region  *region;
    const ConstNodeList *nodelist;
};
}

template <typename DoubleType>
void InterfaceEquation<DoubleType>::NodeVolumeType1Assemble(const std::string &interfacenodemodel, dsMath::RealRowColValueVec<DoubleType> &m, dsMath::RHSEntryVec<DoubleType> &v, PermutationMap &p, dsMathEnum::WhatToLoad w, const std::string &/*surface_area*/)
{
    const Interface &in = GetInterface();
    const Region &r0 = *in.GetRegion0();
    const Region &r1 = *in.GetRegion1();

    std::vector<std::string> vlist0 = r0.GetVariableList();
    std::vector<std::string> vlist1 = r1.GetVariableList();

    const auto &myname = this->GetName();
    const auto &myname0 = this->GetName0();
    const auto &myname1 = this->GetName1();

    const size_t eqindex0 = r0.GetEquationIndex(myname0);
    const size_t eqindex1 = r1.GetEquationIndex(myname1);

    {
      std::ostringstream os;

      if (eqindex0 == size_t(-1))
      {
        os << "Could not find equation " << myname0 << " on region0 " << r0.GetName() << " for interface equation " << myname << "\n";
      }

      if (eqindex1 == size_t(-1))
      {
        os << "Could not find equation " << myname1 << " on region1 " << r1.GetName() << " for interface equation " << myname << "\n";
      }

      if (interfacenodemodel.empty())
      {
        os << "Interface node model is empty\n";
      }

      if (!os.str().empty())
      {
        GeometryStream::WriteOut(OutputStream::OutputType::FATAL, GetInterface(), std::string("On interface ") + GetInterface().GetName() + "\n" + os.str());
      }

    }

    ConstInterfaceNodeModelPtr inm = in.GetInterfaceNodeModel(interfacenodemodel);

    {
      std::ostringstream os;

      if (!inm.get())
      {
        os << "Interface node model " << interfacenodemodel << " is not available\n";
      }

      if (!os.str().empty())
      {
        GeometryStream::WriteOut(OutputStream::OutputType::FATAL, GetInterface(), std::string("On interface ") + GetInterface().GetName() + "\n" + os.str());
      }

    }

    const std::set<ConstNodePtr> &activeNodes0 = GetActiveNodes0();
    const ConstNodeList_t &nodes0 = in.GetNodes0();

    const std::set<ConstNodePtr> &activeNodes1 = GetActiveNodes1();
    const ConstNodeList_t &nodes1 = in.GetNodes1();

    // technically this is the responsibility of the interface to check
    // We need to combine get active nodes for this
    dsAssert(nodes0.size() == nodes1.size(), "UNEXPECTED");

    /// we will permute out bulk equations for node1 into node0
    /// we will assemble our interface equation at node1
    const NodeScalarList<DoubleType> rhs = inm->GetScalarValues<DoubleType>();
    dsAssert(rhs.size() == nodes1.size(), "UNEXPECTED");
    for (size_t i = 0; i < nodes0.size(); ++i)
    {
        const Node *node0 = nodes0[i];
        const Node *node1 = nodes1[i];

        if (!(activeNodes0.count(node0) && activeNodes1.count(node1)))
        {
          continue;
        }

        /// Assemble all fluxes into this equation
        const size_t row0 = r0.GetEquationNumber(eqindex0, node0);
        /// Assemble our interface equation here
        const size_t row1 = r1.GetEquationNumber(eqindex1, node1);

        dsAssert(row0 != size_t(-1), "UNEXPECTED");
        dsAssert(row1 != size_t(-1), "UNEXPECTED");

        if (w == dsMathEnum::WhatToLoad::PERMUTATIONSONLY)
        {
            p[row1] = PermutationEntry(row0, false);
        }
        else if ((w == dsMathEnum::WhatToLoad::RHS) || (w == dsMathEnum::WhatToLoad::MATRIXANDRHS))
        {
            v.push_back(std::make_pair(row1,  rhs[i]));
        }
        else if (w == dsMathEnum::WhatToLoad::MATRIXONLY)
        {
        }
        else
        {
        }
    }

    if (w == dsMathEnum::WhatToLoad::PERMUTATIONSONLY)
    {
    }
    else if ((w == dsMathEnum::WhatToLoad::MATRIXONLY) || (w == dsMathEnum::WhatToLoad::MATRIXANDRHS))
    {
        // variable list
        std::vector<vlistdata> vlistd;
        vlistd.reserve(vlist0.size() + vlist1.size());
        for (size_t i = 0; i < vlist0.size(); ++i)
        {
            const std::string &var = vlist0[i];

            std::string dermodel = interfacenodemodel;
            dermodel += ":";
            dermodel += var;
            dermodel += "@r0";
            ConstInterfaceNodeModelPtr idm = in.GetInterfaceNodeModel(dermodel);
            if (!idm)
            {
                dsErrors::MissingInterfaceEquationModel(r0, *this, dermodel, OutputStream::OutputType::VERBOSE1);
            }
            else
            {
                vlistd.push_back(vlistdata(dermodel, var, r0, nodes0));
            }
        }
        for (size_t i = 0; i < vlist1.size(); ++i)
        {
            const std::string &var = vlist1[i];

            std::string dermodel = interfacenodemodel;
            dermodel += ":";
            dermodel += var;
            dermodel += "@r1";
            ConstInterfaceNodeModelPtr idm = in.GetInterfaceNodeModel(dermodel);
            if (!idm)
            {
                dsErrors::MissingInterfaceEquationModel(r1, *this, dermodel, OutputStream::OutputType::VERBOSE1);
            }
            else
            {
                vlistd.push_back(vlistdata(dermodel, var, r1, nodes1));
            }
        }

        for (size_t j = 0; j < vlistd.size(); ++j)
        {
            const std::string   &imname = vlistd[j].interfacemodelname;
            const std::string   &var    = vlistd[j].var;
            const Region        &reg    = *vlistd[j].region;
            const ConstNodeList &nlist  = *vlistd[j].nodelist;

            const size_t eqindex = reg.GetEquationIndex(reg.GetEquationNameFromVariable(var));
            dsAssert(eqindex != size_t(-1), "UNEXPECTED");

            ConstInterfaceNodeModelPtr idm = in.GetInterfaceNodeModel(imname);
            dsAssert(idm.get(), "UNEXPECTED"); // Existence is checked when added to vlistd

            const NodeScalarList<DoubleType> &vals = idm->GetScalarValues<DoubleType>();
            dsAssert(vals.size() == nlist.size(), "UNEXPECTED");

            for (size_t i = 0; i < nlist.size(); ++i)
            {
                const size_t col = reg.GetEquationNumber(eqindex, nlist[i]);
                dsAssert(col != size_t(-1), "UNEXPECTED");

                const Node *node0 = nodes0[i];
                const Node *node1 = nodes1[i];

                if (!(activeNodes0.count(node0) && (activeNodes1.count(node1))))
                {
                  continue;
                }

                // as stated previously, we are assembling into the second region
                const size_t row1 = r1.GetEquationNumber(eqindex1, node1);
                const DoubleType val = vals[i];

                m.push_back(dsMath::RealRowColVal<DoubleType>(row1, col, val));
            }
        }
    }
    else if (w == dsMathEnum::WhatToLoad::RHS)
    {
    }
    else
    {
        dsAssert(0, "UNEXPECTED");
    }
}

template <typename DoubleType>
void InterfaceEquation<DoubleType>::NodeVolumeType2Assemble(const std::string &interfacenodemodel, dsMath::RealRowColValueVec<DoubleType> &m, dsMath::RHSEntryVec<DoubleType> &v, PermutationMap &p, dsMathEnum::WhatToLoad w, const std::string &surface_area)
{
    //// special short cut
    if (w == dsMathEnum::WhatToLoad::PERMUTATIONSONLY)
    {
        return;
    }

    const Interface &in = GetInterface();
    const Region &r0 = *in.GetRegion0();
    const Region &r1 = *in.GetRegion1();

    std::vector<std::string> vlist0 = r0.GetVariableList();
    std::vector<std::string> vlist1 = r1.GetVariableList();

#if 0
    const auto &myname  = this->GetName();
#endif
    const auto &myname0 = this->GetName0();
    const auto &myname1 = this->GetName1();

    const size_t eqindex0 = r0.GetEquationIndex(myname0);
    const size_t eqindex1 = r1.GetEquationIndex(myname1);

    /// It doesn't make sense if the equation doesn't exist in both regions of interface
    dsAssert(eqindex0 != size_t(-1), "UNEXPECTED");
    dsAssert(eqindex1 != size_t(-1), "UNEXPECTED");
    dsAssert(!interfacenodemodel.empty(), "UNEXPECTED");

    ConstInterfaceNodeModelPtr inm = in.GetInterfaceNodeModel(interfacenodemodel);
    dsAssert(inm.get(), "UNEXPECTED");
    ConstNodeModelPtr sa0 = r0.GetNodeModel(surface_area);
    dsAssert(sa0.get(), "UNEXPECTED");

    ConstNodeModelPtr sa1 = r1.GetNodeModel(surface_area);
    dsAssert(sa1.get(), "UNEXPECTED");

    const std::set<ConstNodePtr> &activeNodes0 = GetActiveNodes0();
    const ConstNodeList_t &nodes0 = in.GetNodes0();

    const std::set<ConstNodePtr> &activeNodes1 = GetActiveNodes1();
    const ConstNodeList_t &nodes1 = in.GetNodes1();

    // technically this is the responsibility of the interface to check
    dsAssert(nodes0.size() == nodes1.size(), "UNEXPECTED");

    /// we will permute out bulk equations for node1 into node0
    /// we will assemble our interface equation at node1
    const NodeScalarList<DoubleType> rhs = inm->GetScalarValues<DoubleType>();
    dsAssert(rhs.size() == nodes1.size(), "UNEXPECTED");

    if ((w == dsMathEnum::WhatToLoad::RHS) || (w == dsMathEnum::WhatToLoad::MATRIXANDRHS))
    {
        const NodeScalarList<DoubleType> &sa0vals = sa0->GetScalarValues<DoubleType>();
        const NodeScalarList<DoubleType> &sa1vals = sa1->GetScalarValues<DoubleType>();

        for (size_t i = 0; i < nodes0.size(); ++i)
        {

            const Node *node0 = nodes0[i];
            const Node *node1 = nodes1[i];

            if (!(activeNodes0.count(node0) && activeNodes1.count(node1)))
            {
              continue;
            }

            /// Assemble all fluxes into this equation
            const size_t row0 = r0.GetEquationNumber(eqindex0, node0);
            /// Assemble our interface equation here
            const size_t row1 = r1.GetEquationNumber(eqindex1, node1);

            dsAssert(row0 != size_t(-1), "UNEXPECTED");
            dsAssert(row1 != size_t(-1), "UNEXPECTED");

            const DoubleType rval = rhs[i];

            const DoubleType val0 =  rval * sa0vals[node0->GetIndex()];
            const DoubleType val1 = -rval * sa1vals[node1->GetIndex()];

            v.push_back(std::make_pair(row0, val0));
            v.push_back(std::make_pair(row1, val1));
        }
    }

    if ((w == dsMathEnum::WhatToLoad::MATRIXONLY) || (w == dsMathEnum::WhatToLoad::MATRIXANDRHS))
    {
        // variable list
        std::vector<vlistdata> vlistd;
        vlistd.reserve(vlist0.size() + vlist1.size());
        for (size_t i = 0; i < vlist0.size(); ++i)
        {
            const std::string &var = vlist0[i];

            std::string dermodel = interfacenodemodel;
            dermodel += ":";
            dermodel += var;
            dermodel += "@r0";
            ConstInterfaceNodeModelPtr idm = in.GetInterfaceNodeModel(dermodel);
            if (!idm)
            {
                dsErrors::MissingInterfaceEquationModel(r0, *this, dermodel, OutputStream::OutputType::VERBOSE1);
            }
            else
            {
                vlistd.push_back(vlistdata(dermodel, var, r0, nodes0));
            }
        }
        for (size_t i = 0; i < vlist1.size(); ++i)
        {
            const std::string &var = vlist1[i];

            std::string dermodel = interfacenodemodel;
            dermodel += ":";
            dermodel += var;
            dermodel += "@r1";
            ConstInterfaceNodeModelPtr idm = in.GetInterfaceNodeModel(dermodel);
            if (!idm)
            {
                dsErrors::MissingInterfaceEquationModel(r1, *this, dermodel, OutputStream::OutputType::VERBOSE1);
            }
            else
            {
                vlistd.push_back(vlistdata(dermodel, var, r1, nodes1));
            }
        }

        for (size_t j = 0; j < vlistd.size(); ++j)
        {
            const std::string   &imname = vlistd[j].interfacemodelname;
            const std::string   &var    = vlistd[j].var;
            const Region        &reg    = *vlistd[j].region;
            const ConstNodeList &nlist  = *vlistd[j].nodelist;

            const size_t eqindex = reg.GetEquationIndex(reg.GetEquationNameFromVariable(var));
            dsAssert(eqindex != size_t(-1), "UNEXPECTED");

            ConstInterfaceNodeModelPtr idm = in.GetInterfaceNodeModel(imname);
            dsAssert(idm.get(), "UNEXPECTED"); // Existence is checked when added to vlistd

            const NodeScalarList<DoubleType> &vals = idm->GetScalarValues<DoubleType>();
            dsAssert(vals.size() == nlist.size(), "UNEXPECTED");

            const NodeScalarList<DoubleType> &sa0vals = sa0->GetScalarValues<DoubleType>();
            const NodeScalarList<DoubleType> &sa1vals = sa1->GetScalarValues<DoubleType>();

            for (size_t i = 0; i < nlist.size(); ++i)
            {
                if (!(activeNodes0.count(nodes0[i]) && activeNodes1.count(nodes1[i])))
                {
                  continue;
                }

                const Node *node0 = nodes0[i];
                const Node *node1 = nodes1[i];

                const size_t col = reg.GetEquationNumber(eqindex, nlist[i]);
                dsAssert(col != size_t(-1), "UNEXPECTED");

                // as stated previously, we are assembling into the second region
                const size_t row0 = r0.GetEquationNumber(eqindex0, node0);
                const size_t row1 = r1.GetEquationNumber(eqindex1, node1);

                const DoubleType rval = vals[i];

                const DoubleType val0 =  rval * sa0vals[node0->GetIndex()];
                const DoubleType val1 = -rval * sa1vals[node1->GetIndex()];

                m.push_back(dsMath::RealRowColVal<DoubleType>(row0, col, +val0));
                m.push_back(dsMath::RealRowColVal<DoubleType>(row1, col, +val1));
            }
        }
    }
    else if (w == dsMathEnum::WhatToLoad::RHS)
    {
    }
    else
    {
        dsAssert(0, "UNEXPECTED");
    }
}

template <typename DoubleType>
void InterfaceEquation<DoubleType>::NodeVolumeType3Assemble(const std::string &interfacenodemodel, dsMath::RealRowColValueVec<DoubleType> &m, dsMath::RHSEntryVec<DoubleType> &v, PermutationMap &p, dsMathEnum::WhatToLoad w, const std::string &surface_area)
{

    const Interface &in = GetInterface();
    const Region &r0 = *in.GetRegion0();
    const Region &r1 = *in.GetRegion1();

    std::vector<std::string> vlist0 = r0.GetVariableList();
    std::vector<std::string> vlist1 = r1.GetVariableList();

    const auto &myname0 = this->GetName0();
    const auto &myname1 = this->GetName1();

    const size_t eqindex0 = r0.GetEquationIndex(myname0);
    const size_t eqindex1 = r1.GetEquationIndex(myname1);

    /// It doesn't make sense if the equation doesn't exist in both regions of interface
    dsAssert(eqindex0 != size_t(-1), "UNEXPECTED");
    dsAssert(eqindex1 != size_t(-1), "UNEXPECTED");
    dsAssert(!interfacenodemodel.empty(), "UNEXPECTED");

    ConstInterfaceNodeModelPtr inm = in.GetInterfaceNodeModel(interfacenodemodel);
    dsAssert(inm.get(), "UNEXPECTED");
    ConstNodeModelPtr sa0 = r0.GetNodeModel(surface_area);
    dsAssert(sa0.get(), "UNEXPECTED");

    ConstNodeModelPtr sa1 = r1.GetNodeModel(surface_area);
    dsAssert(sa1.get(), "UNEXPECTED");

    const std::set<ConstNodePtr> &activeNodes0 = GetActiveNodes0();
    const std::set<ConstNodePtr> &activeNodes1 = GetActiveNodes1();

    const ConstNodeList_t &nodes0 = in.GetNodes0();
    const ConstNodeList_t &nodes1 = in.GetNodes1();

    // technically this is the responsibility of the interface to check
    dsAssert(nodes0.size() == nodes1.size(), "UNEXPECTED");

    /// we will permute out bulk equations for node1 into node0
    /// we will assemble our interface equation at node1
    const NodeScalarList<DoubleType> rhs = inm->GetScalarValues<DoubleType>();
    dsAssert(rhs.size() == nodes1.size(), "UNEXPECTED");

#if 0
    const NodeScalarList<DoubleType> &sa0vals = sa0->GetScalarValues<DoubleType>();
#endif
    const NodeScalarList<DoubleType> &sa1vals = sa1->GetScalarValues<DoubleType>();

    for (size_t i = 0; i < nodes0.size(); ++i)
    {

        const Node *node0 = nodes0[i];
        const Node *node1 = nodes1[i];

        if (!(activeNodes0.count(node0) && activeNodes1.count(node1)))
        {
          continue;
        }

        const size_t row0 = r0.GetEquationNumber(eqindex0, node0);
        const size_t row1 = r1.GetEquationNumber(eqindex1, node1);

        dsAssert(row0 != size_t(-1), "UNEXPECTED");
        dsAssert(row1 != size_t(-1), "UNEXPECTED");

        if (w == dsMathEnum::WhatToLoad::PERMUTATIONSONLY)
        {
          // keeps an unpermutated copy
          p[row1] = PermutationEntry(row0, true);
        }
        else if ((w == dsMathEnum::WhatToLoad::RHS) || (w == dsMathEnum::WhatToLoad::MATRIXANDRHS))
        {
          const DoubleType rval = rhs[i];

#if 0
          const DoubleType val0 =  rval * sa0vals[node0->GetIndex()];
#endif
          const DoubleType val1 = -rval * sa1vals[node1->GetIndex()];

#if 0
          v.push_back(std::make_pair(row0, val0));
#endif
          v.push_back(std::make_pair(row1, val1));
        }
    }

    if ((w == dsMathEnum::WhatToLoad::MATRIXONLY) || (w == dsMathEnum::WhatToLoad::MATRIXANDRHS))
    {
        // variable list
        std::vector<vlistdata> vlistd;
        vlistd.reserve(vlist0.size() + vlist1.size());
        for (size_t i = 0; i < vlist0.size(); ++i)
        {
            const std::string &var = vlist0[i];

            std::string dermodel = interfacenodemodel;
            dermodel += ":";
            dermodel += var;
            dermodel += "@r0";
            ConstInterfaceNodeModelPtr idm = in.GetInterfaceNodeModel(dermodel);
            if (!idm)
            {
                dsErrors::MissingInterfaceEquationModel(r0, *this, dermodel, OutputStream::OutputType::VERBOSE1);
            }
            else
            {
                vlistd.push_back(vlistdata(dermodel, var, r0, nodes0));
            }
        }
        for (size_t i = 0; i < vlist1.size(); ++i)
        {
            const std::string &var = vlist1[i];

            std::string dermodel = interfacenodemodel;
            dermodel += ":";
            dermodel += var;
            dermodel += "@r1";
            ConstInterfaceNodeModelPtr idm = in.GetInterfaceNodeModel(dermodel);
            if (!idm)
            {
                dsErrors::MissingInterfaceEquationModel(r1, *this, dermodel, OutputStream::OutputType::VERBOSE1);
            }
            else
            {
                vlistd.push_back(vlistdata(dermodel, var, r1, nodes1));
            }
        }

        for (size_t j = 0; j < vlistd.size(); ++j)
        {
            const std::string   &imname = vlistd[j].interfacemodelname;
            const std::string   &var    = vlistd[j].var;
            const Region        &reg    = *vlistd[j].region;
            const ConstNodeList &nlist  = *vlistd[j].nodelist;

            const size_t eqindex = reg.GetEquationIndex(reg.GetEquationNameFromVariable(var));
            dsAssert(eqindex != size_t(-1), "UNEXPECTED");

            ConstInterfaceNodeModelPtr idm = in.GetInterfaceNodeModel(imname);
            dsAssert(idm.get(), "UNEXPECTED"); // Existence is checked when added to vlistd

            const NodeScalarList<DoubleType> &vals = idm->GetScalarValues<DoubleType>();
            dsAssert(vals.size() == nlist.size(), "UNEXPECTED");

#if 0
            const NodeScalarList<DoubleType> &sa0vals = sa0->GetScalarValues<DoubleType>();
#endif
            const NodeScalarList<DoubleType> &sa1vals = sa1->GetScalarValues<DoubleType>();

            for (size_t i = 0; i < nlist.size(); ++i)
            {
                if (!(activeNodes0.count(nodes0[i]) && activeNodes1.count(nodes1[i])))
                {
                  continue;
                }

#if 0
                const Node *node0 = nodes0[i];
#endif
                const Node *node1 = nodes1[i];

                const size_t col = reg.GetEquationNumber(eqindex, nlist[i]);
                dsAssert(col != size_t(-1), "UNEXPECTED");

                // as stated previously, we are assembling into the second region
#if 0
                const size_t row0 = r0.GetEquationNumber(eqindex0, node0);
#endif
                const size_t row1 = r1.GetEquationNumber(eqindex1, node1);

                const DoubleType rval = vals[i];

#if 0
                const DoubleType val0 =  rval * sa0vals[node0->GetIndex()];
#endif
                const DoubleType val1 = -rval * sa1vals[node1->GetIndex()];
#if 0
                m.push_back(dsMath::RealRowColVal<DoubleType>(row0, col, +val0));
#endif
                m.push_back(dsMath::RealRowColVal<DoubleType>(row1, col, +val1));
            }
        }
    }
    else if (w == dsMathEnum::WhatToLoad::RHS || w == dsMathEnum::WhatToLoad::PERMUTATIONSONLY)
    {
    }
    else
    {
        dsAssert(0, "UNEXPECTED");
    }
}

template <typename DoubleType>
void InterfaceEquation<DoubleType>::DevsimSerialize(std::ostream &of) const
{
  of << "begin_equation \"" << this->GetName() << "\"\n";
  this->Serialize(of);
  of << "\nend_equation\n\n";
}

template <typename DoubleType>
void InterfaceEquation<DoubleType>::GetCommandOptions(std::map<std::string, ObjectHolder> &omap) const
{
  this->GetCommandOptions_Impl(omap);
}

template class InterfaceEquation<double>;

#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class InterfaceEquation<float128>;
#endif

