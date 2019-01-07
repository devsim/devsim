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

//// We need to move this class out of this directory to one for storing global data
//// This directory should not depend on the meshing directory
#include "GlobalData.hh"
#include "Device.hh"
#include "Region.hh"
#include "OutputStream.hh"
#include "dsAssert.hh"
#include "ObjectHolder.hh"

#include <sstream>


GlobalData *GlobalData::instance = 0;

GlobalData::GlobalData()
{
  InitializeParameters();
}

GlobalData &GlobalData::GetInstance()
{
    if (!instance)
    {
        instance = new GlobalData;
    }
    return *instance;
}

void GlobalData::DestroyInstance()
{
    if (instance)
    {
        delete instance;
    }
    instance = 0;
}

GlobalData::~GlobalData()
{
  DeviceList_t::iterator dit = deviceList.begin();
  for ( ; dit != deviceList.end(); ++dit)
  {
      delete dit->second;
  }
}

void GlobalData::AddDevice(DevicePtr dp)
{
    /// Instead of assertions, we should return a flag
    const std::string &nm = dp->GetName();
    dsAssert(deviceList.count(nm) == 0, "UNEXPECTED");
    deviceList[nm]=dp;
}

DevicePtr GlobalData::GetDevice(const std::string &nm)
{
    Device *dp = nullptr;
    if (deviceList.count(nm))
    {
        dp = deviceList[nm];
    }
    return dp;
}

//// Need callback system when material parameters change
void GlobalData::AddDBEntryOnDevice(const std::string &device, const std::string &name, ObjectHolder value)
{
    deviceData[device][name] = value;

    SignalCallbacksOnDevice(device, name);
}

void GlobalData::AddDBEntryOnRegion(const std::string &device, const std::string &region, const std::string &name, ObjectHolder value)
{
    regionData[device][region][name] = value;
    SignalCallbacksOnRegion(device, region, name);
}

void GlobalData::AddDBEntryOnRegion(const Region *rp, const std::string &name, ObjectHolder value)
{
    const std::string &device = rp->GetDevice()->GetName();
    const std::string &region = rp->GetName();
    AddDBEntryOnRegion(device, region, name, value);
    SignalCallbacksOnRegion(device, region, name);
}

void GlobalData::AddDBEntryOnGlobal(const std::string &name, ObjectHolder value)
{
    globalData[name] = value;
    SignalCallbacksOnGlobal(name);
}

GlobalData::DBEntry_t GlobalData::GetDBEntryOnDevice(const std::string &device, const std::string &name) const
{
  DBEntry_t ret = std::make_pair(false, ObjectHolder());

  DeviceDataMap_t::const_iterator dit = deviceData.find(device); 
  if (dit != deviceData.end())
  {
    const GlobalDataMap_t &gdm = (*dit).second;
    GlobalDataMap_t::const_iterator git = gdm.find(name);
    if (git != gdm.end())
    {
      const ObjectHolder value = (*git).second;
      ret = std::make_pair(true, value);
    }
  }

  if (!ret.first)
  {
    ret = GetDBEntryOnGlobal(name);
  }

  return ret;
}

GlobalData::DBEntry_t GlobalData::GetDBEntryOnRegion(const std::string &device, const std::string &region, const std::string &name) const
{
  DBEntry_t ret = std::make_pair(false, ObjectHolder());

  RegionDataMap_t::const_iterator rit = regionData.find(device);
  if (rit != regionData.end())
  {
    const DeviceDataMap_t &ddm = (*rit).second;
    const DeviceDataMap_t::const_iterator dit = ddm.find(region); 
    if (dit != ddm.end())
    {
      const GlobalDataMap_t &gdm = (*dit).second;
      GlobalDataMap_t::const_iterator git = gdm.find(name);
      if (git != gdm.end())
      {
        const ObjectHolder value = (*git).second;
        ret = std::make_pair(true, value);
      }
    }
  }

  if (!ret.first)
  {
    ret  = GetDBEntryOnDevice(device, name);
  }

  if (!ret.first)
  {
    ret = GetDBEntryOnGlobal(name);
  }
  return ret;
}

GlobalData::DBEntry_t GlobalData::GetDBEntryOnRegion(const Region *rp, const std::string &name) const
{
  const std::string &device = rp->GetDevice()->GetName();
  const std::string &region = rp->GetName();
  return GetDBEntryOnRegion(device, region, name);
}

GlobalData::DBEntry_t GlobalData::GetDBEntryOnGlobal(const std::string &name) const
{
  DBEntry_t ret = std::make_pair(false, ObjectHolder());
  GlobalDataMap_t::const_iterator git = globalData.find(name);
  if (git != globalData.end())
  {
    const ObjectHolder value = (*git).second;
    ret = std::make_pair(true, value);
  }
  return ret;
}

GlobalData::DoubleDBEntry_t GlobalData::GetDoubleDBEntryOnRegion(const Region *rp, const std::string &name) const
{
  DoubleDBEntry_t ret = std::make_pair(false, 0.0);
  DBEntry_t       dbent = GetDBEntryOnRegion(rp, name);
  if (dbent.first)
  {
    ObjectHolder::DoubleEntry_t dval = dbent.second.GetDouble();
    if (dval.first)
    {
      ret = dval;
    }
    else
    {
      const std::string &region = rp->GetName();
      const std::string &device = rp->GetDevice()->GetName();
      std::ostringstream os; 
      os << "Device \"" << device << 
         "\" Region \"" << region
         << "\" database entry \"" << name
         << "\" resolves to a string \"" << dbent.second.GetString() <<  "\" when a number was expected\n";
      OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
    }
  }
  return ret;
}

std::vector<std::string> GlobalData::GetDBEntryListOnGlobal() const
{
  std::vector<std::string> ret;
  ret.reserve(globalData.size());
  for (GlobalDataMap_t::const_iterator git = globalData.begin(); git != globalData.end(); ++git)
  {
    ret.push_back((*git).first);
  }
  return ret;
}

std::vector<std::string> GlobalData::GetDBEntryListOnDevice(const std::string &device) const
{
  std::vector<std::string> ret;
  DeviceDataMap_t::const_iterator dit = deviceData.find(device);
  if (dit != deviceData.end())
  {
    const GlobalDataMap_t &gdm = (*dit).second;
    ret.reserve(gdm.size());
    for (GlobalDataMap_t::const_iterator git = gdm.begin(); git != gdm.end(); ++git)
    {
      ret.push_back((*git).first);
    }
  }
  return ret;
}

std::vector<std::string> GlobalData::GetDBEntryListOnRegion(const std::string &device, const std::string &region) const
{
  std::vector<std::string> ret;
  RegionDataMap_t::const_iterator rit = regionData.find(device);
  if (rit != regionData.end())
  {
    const DeviceDataMap_t &ddm = (*rit).second;
    DeviceDataMap_t::const_iterator dit = ddm.find(region);
    if (dit != ddm.end())
    {
      const GlobalDataMap_t &gdm = (*dit).second;
      ret.reserve(gdm.size());
      for (GlobalDataMap_t::const_iterator git = gdm.begin(); git != gdm.end(); ++git)
      {
        ret.push_back((*git).first);
      }
    }
  }
  return ret;
}

void GlobalData::SignalCallbacksOnGlobal(const std::string &name)
{
    for (DeviceList_t::iterator it = deviceList.begin(); it != deviceList.end(); ++it)
    {
        const std::string &device = it->first;
//      os << "signal " << device << " " << name << std::endl;
        if (deviceData.count(device))
        {
            if (deviceData[device].count(name))
            {
                std::ostringstream os; 
                os << "Device \"" << device << " shadows global parameter update " << name << "\n";
                OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
            }
            else
            {
//              os << __LINE__ << " signal " << device << " " << name << std::endl;
                SignalCallbacksOnDevice(device, name);
            }
        }
        else
        {
//              os << __LINE__ << " signal " << device << " " << name << std::endl;
                SignalCallbacksOnDevice(device, name);
        }
    }
}

void GlobalData::SignalCallbacksOnDevice(const std::string &device, const std::string &name)
{
//    os  << __LINE__ << " signal " << device << " " << name << std::endl;
    DeviceList_t::iterator it = deviceList.find(device);
    if (it != deviceList.end())
    {
        const Device::RegionList_t &rlist = (it->second)->GetRegionList();
        for (Device::RegionList_t::const_iterator rit = rlist.begin(); rit != rlist.end(); ++rit)
        {
            const std::string &region = rit->first;
            if (regionData.count(device))
            {
                if (regionData[device].count(region))
                {
                    if (regionData[device][region].count(name))
                    {
                        std::ostringstream os; 
                        os << "Device \"" << device << 
                                     "\" Region \"" << region
                                     << "\" shadows global parameter update " << name << "\n";
                        OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
                    }
                    else
                    {
//                      os  << __LINE__ << " signal " << device << " " << region << " " << name << std::endl;
                        (rit->second)->SignalCallbacks(name);
                    }
                }
                else
                {
//                  os  << __LINE__ << " signal " << device << " " << region << " " << name << std::endl;
                    (rit->second)->SignalCallbacks(name);
                }
            }
            else
            {
//              os  << __LINE__ << " signal " << device << " " << region << " " << name << std::endl;
                (rit->second)->SignalCallbacks(name);
            }
        }
    }
}

void GlobalData::SignalCallbacksOnRegion(const std::string &device, const std::string &region, const std::string &name)
{
    DeviceList_t::iterator it = deviceList.find(device);
    if (it != deviceList.end())
    {
    ///// Need to assert when parameters are being shadowed.
        const RegionPtr rp = (it->second)->GetRegion(region);
        if (rp)
        {
            rp->SignalCallbacks(name);
        }
    }
}

void GlobalData::SetInterpreter(void *t)
{
  tclInterp = t;
}

void *GlobalData::GetInterpreter()
{
  return tclInterp;
}

///// We need to mask when stuff is specified on a device or global basis
void GlobalData::SignalCallbacksOnMaterialChange(const std::string &material_name, const std::string &parameter_name)
{

  std::vector<Region *> region_list;

  for (DeviceList_t::iterator dit = deviceList.begin(); dit != deviceList.end(); ++dit)
  {
    const Device::RegionList_t &rlist = (dit->second)->GetRegionList();
    for (Device::RegionList_t::const_iterator rit = rlist.begin(); rit != rlist.end(); ++rit)
    {
      region_list.push_back(rit->second);
    }
  }

  for (std::vector<Region *>::const_iterator it = region_list.begin(); it != region_list.end(); ++it)
  {
    Region *rp = *it;

    const std::string &mname = rp->GetMaterialName();

    if ((mname == material_name) || (material_name == "global"))
    {
      DBEntry_t dbent = GetDBEntryOnRegion(rp, parameter_name);

      if (!dbent.first)
      {
        rp->SignalCallbacks(parameter_name);
      }
      else
      {
        const std::string &region = rp->GetName();
        const std::string &device = rp->GetDevice()->GetName();
        std::ostringstream os; 
        os << "Device \"" << device << 
           "\" Region \"" << region <<
           "\" Material \"" << material_name
           << "\" material db entry \"" << parameter_name
           << "\" is masked by parameter setting.\n";
        OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
      }
    }
  }
}

bool GlobalData::AddTclEquation(const std::string &name, const std::string &procedure, std::string &error)
{
  bool ret = false;
  if (tclEquationList.count(name))
  {
    error += std::string("Tcl equation \"") + name + "\" already exists";
  }
  else
  {
    ret = true;
    tclEquationList[name] = procedure;
  }
  return ret;
}

const GlobalData::TclEquationList_t &GlobalData::GetTclEquationList()
{
  return tclEquationList;
}

void GlobalData::InitializeParameters()
{
  globalData["node_volume_model"] = ObjectHolder(std::string("NodeVolume"));
  globalData["edge_couple_model"] = ObjectHolder(std::string("EdgeCouple"));
  globalData["element_edge_couple_model"] = ObjectHolder(std::string("ElementEdgeCouple"));
  globalData["element_node0_volume_model"] = ObjectHolder(std::string("ElementNodeVolume"));
  globalData["element_node1_volume_model"] = ObjectHolder(std::string("ElementNodeVolume"));
  globalData["surface_area_model"] = ObjectHolder(std::string("SurfaceArea"));
}


