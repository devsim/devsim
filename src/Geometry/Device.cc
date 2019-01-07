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

#include "Device.hh"
#include "Region.hh"
#include "Contact.hh"
#include "ContactEquation.hh"
#include "ContactEquationHolder.hh"
#include "Equation.hh"
#include "Coordinate.hh"
#include "Interface.hh"
#include "InterfaceEquation.hh"
#include "dsAssert.hh"
#include "OutputStream.hh"
#include "Node.hh"
#include "GeometryStream.hh"
#include <algorithm>
#include <vector>

Device::Device(std::string devname, size_t dim)
    : baseeqnnum(size_t(-1)), relError(0.0), absError(0.0)
{
   dsAssert(!devname.empty(), "UNEXPECTED");
   deviceName = devname;
   dsAssert(dim >=1 && dim <= 3, "UNEXPECTED");
   dimension = dim;
}

/// If we ever use smart pointers we don't need to call deleters explicitly
Device::~Device()
{
    {
        RegionList_t::iterator rit = regionList.begin();
        for ( ; rit != regionList.end(); ++rit)
        {
            delete rit->second;
        }
    }

    {
        ContactList_t::iterator cit = contactList.begin();
        for ( ; cit != contactList.end(); ++cit)
        {
            delete cit->second;
        }
    }

    {
        InterfaceList_t::iterator iit = interfaceList.begin();
        for ( ; iit != interfaceList.end(); ++iit)
        {
            delete iit->second;
        }
    }

    {
        for (size_t i=0 ; i < coordinateList.size(); ++i)
        {
            delete coordinateList[i];
        }
    }
    // TODO: use DeletePointers template here
}

// Note that a nd can have an index of 0
// Need to minimize reallocations by setting a default minimum size
// and letting mesher specify the number of nodes to be added

void Device::AddRegion(const RegionPtr &rp)
{
   // Should make map with name or just iterate to find 
   dsAssert(rp->GetDimension() == dimension, "UNEXPECTED");
   const std::string &nm = rp->GetName();

   // There can be only one by this name
   dsAssert(regionList.count(nm) == 0, "UNEXPECTED");
   regionList[nm]=rp;
}

void Device::AddContact(const ContactPtr &cp)
{
   const std::string &nm = cp->GetName();
   // There can be only one
   dsAssert(contactList.count(nm) == 0, "UNEXPECTED");
   contactList[nm]=cp;

   ConstRegionPtr crp = cp->GetRegion();
   (const_cast<RegionPtr>(crp))->SignalCallbacks("@@@ContactChange");

  //// Make sure the reverse operation is done if we ever remove a contact
  const ConstNodeList_t &cnodes = cp->GetNodes();

  std::map<std::string, size_t> warning_list;
  for (ConstNodeList_t::const_iterator it = cnodes.begin(); it != cnodes.end(); ++it)
  {
    const size_t ci = (*it)->GetCoordinate().GetIndex();

    if (coordinateIndexToContact.count(ci) &&  coordinateIndexToContact[ci].begin() != coordinateIndexToContact[ci].end())
    {
      std::ostringstream os;
      os << "Warning, contact \"" << (*coordinateIndexToContact[ci].begin())->GetName() << "\" shares a node with contact \"" << nm << "\"";
      warning_list[os.str()] += 1;
    }

    if (coordinateIndexToInterface.count(ci) &&  coordinateIndexToInterface[ci].begin() != coordinateIndexToInterface[ci].end())
    {
      std::ostringstream os;
      os << "Warning, interface \"" << (*coordinateIndexToInterface[ci].begin())->GetName() << "\" shares a node with contact \"" << nm << "\"";
      warning_list[os.str()] += 1;
    }

    //// Guarantee that vector is sorted based on pointer address
    std::vector<ContactPtr> &cplist = coordinateIndexToContact[ci];
    cplist.push_back(cp);
    std::sort(cplist.begin(), cplist.end());
  }
  if (!warning_list.empty())
  {
    std::ostringstream os;
    for (std::map<std::string, size_t>::iterator it = warning_list.begin(); it != warning_list.end(); ++it)
    {
      os << (*it).first;
      size_t num = (*it).second;
      if (num > 1)
      {
        os << " (repeated " << num - 1 << " times)";
      }
      os << "\n";
    }
    GeometryStream::WriteOut(OutputStream::OutputType::INFO, *this, os.str());
  }
}

void Device::AddInterface(const InterfacePtr &ip)
{
   const std::string &nm = ip->GetName();
   // There can be only one
   dsAssert(interfaceList.count(nm) == 0, "UNEXPECTED");

   interfaceList[nm]=ip;

    
   (const_cast<RegionPtr>(ip->GetRegion0()))->SignalCallbacks("@@@InterfaceChange");
   (const_cast<RegionPtr>(ip->GetRegion1()))->SignalCallbacks("@@@InterfaceChange");

  //// Make sure the reverse operation is done if we ever remove a contact
  const ConstNodeList_t &inodes0 = ip->GetNodes0();
// Tacitly assume that inodes0 coordinate index always matches those on inodes1
//  const ConstNodeList_t &inodes1 = ip->GetNodes1();
  std::map<std::string, size_t> warning_list;
  for (ConstNodeList_t::const_iterator it = inodes0.begin(); it != inodes0.end(); ++it)
  {
    const size_t ii = (*it)->GetCoordinate().GetIndex();

    if (coordinateIndexToInterface.count(ii) &&  coordinateIndexToInterface[ii].begin() != coordinateIndexToInterface[ii].end())
    {
      std::ostringstream os;
      os << "Warning, interface \"" << (*coordinateIndexToInterface[ii].begin())->GetName() << "\" shares a node with interface \"" << nm << "\"";
      warning_list[os.str()] += 1;
    }

    if (coordinateIndexToContact.count(ii) &&  coordinateIndexToContact[ii].begin() != coordinateIndexToContact[ii].end())
    {
      std::ostringstream os;
      os << "Warning, contact \"" << (*coordinateIndexToContact[ii].begin())->GetName() << "\" shares a node with interface \"" << nm << "\"";
      warning_list[os.str()] += 1;
    }

    //// Guarantee that vector is sorted based on pointer address
    std::vector<InterfacePtr> &iplist = coordinateIndexToInterface[ii];
    iplist.push_back(ip);
    std::sort(iplist.begin(), iplist.end());
  }
  if (!warning_list.empty())
  {
    std::ostringstream os;
    for (std::map<std::string, size_t>::iterator it = warning_list.begin(); it != warning_list.end(); ++it)
    {
      os << (*it).first;
      size_t num = (*it).second;
      if (num > 1)
      {
        os << " (repeated " << num - 1 << " times)";
      }
      os << "\n";
    }
    GeometryStream::WriteOut(OutputStream::OutputType::INFO, *this, os.str());
  }
}

void Device::AddCoordinate(CoordinatePtr cp)
{
    coordinateList.push_back(cp);
    cp->setIndex(coordinateList.size()-1);
}

size_t Device::GetNumberOfInterfacesOnCoordinate(const Coordinate &c)
{
  size_t ret = 0;
  CoordinateIndexToInterface_t::iterator it = coordinateIndexToInterface.find(c.GetIndex());
  if (it != coordinateIndexToInterface.end())
  {
    ret = (it->second).size();
  }
  return ret;
}

size_t Device::GetNumberOfContactsOnCoordinate(const Coordinate &c)
{
  size_t ret = 0;
  CoordinateIndexToContact_t::iterator it = coordinateIndexToContact.find(c.GetIndex());
  if (it != coordinateIndexToContact.end())
  {
    ret = (it->second).size();
  }
  return ret;
}

void Device::AddCoordinateList(const CoordinateList_t &cl)
{
    if (coordinateList.empty())
    {
        coordinateList = cl;
        for (size_t i = 0; i < coordinateList.size(); ++i)
        {
            coordinateList[i]->setIndex(i);
        }
    }
    else
    {
        for (size_t i = 0; i < cl.size(); ++i)
        {
            AddCoordinate(cl[i]);
        }
    }
}

void Device::SetBaseEquationNumber(size_t x)
{
    baseeqnnum = x;
}

/// TODO: handle case where there are no nodes on the device
/// TODO: ERROR out when adding equations to regions with no nodes
size_t Device::CalcMaxEquationNumber()
{
    bool hasEquations = false;
    std::ostringstream os; 
    size_t x = baseeqnnum;
    RegionList_t::iterator rit = regionList.begin();
    const RegionList_t::iterator rend = regionList.end();
    for ( ; rit != rend; ++rit)
    {
        const std::string &rname = rit->first;
        Region &region = *(rit->second);

        os << "Region \"" << rname  << "\" on device \"" << deviceName << "\"";
        if (region.GetNumberEquations() != 0)
        {
            region.SetBaseEquationNumber(x);
            const size_t maxnum = region.GetMaxEquationNumber();

            os << " has equations " << x << ":" << maxnum << "\n";

            hasEquations = true;

            x = maxnum + 1;
        }
        else
        {
            os << " has no equations.\n";
        }
    }
    GeometryStream::WriteOut(OutputStream::OutputType::INFO, *this, os.str());

    size_t maxeqnnum = (hasEquations) ? x-1 : size_t(-1);
    return maxeqnnum;
}

template <typename DoubleType>
void Device::ContactAssemble(dsMath::RealRowColValueVec<DoubleType> &m, dsMath::RHSEntryVec<DoubleType> &v, PermutationMap &p, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
  for (auto it : GetContactList())
  {
    it.second->Assemble(m, v, p, w, t);
  }
}

template <typename DoubleType>
void Device::InterfaceAssemble(dsMath::RealRowColValueVec<DoubleType> &m, dsMath::RHSEntryVec<DoubleType> &v, PermutationMap &p, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{

  for (auto it : GetInterfaceList())
  {
    it.second->Assemble(m, v, p, w, t);
  }
}

template <typename DoubleType>
void Device::RegionAssemble(dsMath::RealRowColValueVec<DoubleType> &m, dsMath::RHSEntryVec<DoubleType> &v, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
  for (auto it : GetRegionList())
  {
    it.second->Assemble(m, v, w, t);
  }
}

template <typename DoubleType>
void Device::Update(const std::vector<DoubleType> &result)
{
    relError = 0.0;
    absError = 0.0;
    RegionList_t::iterator it = regionList.begin(); 
    const RegionList_t::iterator end = regionList.end(); 
    for ( ; it != end; ++it)
    {
        Region *rp = it->second;

        rp->Update(result);
        DoubleType rerr = rp->GetRelError<DoubleType>();
        DoubleType aerr = rp->GetAbsError<DoubleType>();

        if (aerr > absError)
        {
            absError = aerr;
        }
        relError += rerr;
    }
}

template <typename DoubleType>
void Device::ACUpdate(const std::vector<std::complex<DoubleType> > &result)
{
    RegionList_t::iterator it = regionList.begin(); 
    const RegionList_t::iterator end = regionList.end(); 
    for ( ; it != end; ++it)
    {
        Region *rp = it->second;

        rp->ACUpdate(result);
    }
}

template <typename DoubleType>
void Device::NoiseUpdate(const std::string &output, const std::vector<PermutationEntry> &permvec, const std::vector<std::complex<DoubleType> > &result)
{
    RegionList_t::iterator it = regionList.begin(); 
    const RegionList_t::iterator end = regionList.end(); 
    for ( ; it != end; ++it)
    {
        Region *rp = it->second;

        rp->NoiseUpdate(output, permvec, result);
    }
}

RegionPtr Device::GetRegion(const std::string &nm)
{
    RegionPtr rp = nullptr;
    if (regionList.count(nm))
    {
        rp = regionList[nm];
    }
    return rp;
}

InterfacePtr Device::GetInterface(const std::string &nm) const
{
  InterfacePtr rp = nullptr;

  InterfaceList_t::const_iterator it = interfaceList.find(nm);

  if (it != interfaceList.end())
  {
    rp = it->second;
  }
  return rp;
}

ContactPtr Device::GetContact(const std::string &nm) const
{
  ContactPtr rp = nullptr;

  ContactList_t::const_iterator it = contactList.find(nm);

  if (it != contactList.end())
  {
    rp = it->second;
  }
  return rp;
}

void Device::BackupSolutions(const std::string &suffix)
{
    RegionList_t::iterator rit = regionList.begin();
    for ( ; rit != regionList.end(); ++rit)
    {
        (rit->second)->BackupSolutions(suffix);
    }
}

void Device::RestoreSolutions(const std::string &suffix)
{
    RegionList_t::iterator rit = regionList.begin();
    for ( ; rit != regionList.end(); ++rit)
    {
        (rit->second)->RestoreSolutions(suffix);
    }
}

void Device::UpdateContacts()
{
  ContactList_t::iterator it = contactList.begin(); 
  const ContactList_t::iterator end = contactList.end(); 
  for ( ; it != end; ++it)
  {
    const ContactEquationPtrMap_t &celist = ((*it).second)->GetEquationPtrList();
    for (ContactEquationPtrMap_t::const_iterator cit = celist.begin(); cit != celist.end(); ++cit)
    {
      ((*cit).second).UpdateContact();
    }
  }
}

void Device::SignalCallbacksOnInterface(const std::string &nm, const Region *rp) const
{
  for (InterfaceList_t::const_iterator it = interfaceList.begin();
        it != interfaceList.end();
        ++it
      )
  {
    it->second->SignalCallbacks(nm, rp);
  }
}

#define DBLTYPE double
#include "DeviceInstantiate.cc"
#undef DBLTYPE
#ifdef DEVSIM_EXTENDED_PRECISION
#define DBLTYPE float128
#include "DeviceInstantiate.cc"
#endif

