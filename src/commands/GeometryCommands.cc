/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "GeometryCommands.hh"
#include "dsCommand.hh"
#include "CommandHandler.hh"
#include "Validate.hh"
#include "GlobalData.hh"
#include "Device.hh"
#include "CheckFunctions.hh"

#include "Contact.hh"
#include "Region.hh"
#include "Interface.hh"
#include "dsAssert.hh"

#include "Tetrahedron.hh"
#include "Triangle.hh"
#include "Edge.hh"
#include "Node.hh"
#include "OutputStream.hh"

#include <sstream>

using namespace dsValidate;

void devsim_initialization();
void ResetAllData();
namespace dsCommand {

void resetDevsimCmd (CommandHandler &data)
{
    std::string errorString;

    /// Will need someway of setting circuit node
    /// (This would be on the contact and not the contact equation??)
    static dsGetArgs::Option option[] =
    {
        {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    OutputStream::WriteOut(OutputStream::OutputType::INFO, "Resetting DEVSIM\n");
    ResetAllData();
    devsim_initialization();
    data.SetEmptyResult();
    return;
}

/// Get the list of all the devices currently loaded
/// There is only an error if there are no devices
void getDeviceListCmd(CommandHandler &data)
{
    std::string errorString;

    /// Will need someway of setting circuit node
    /// (This would be on the contact and not the contact equation??)
    static dsGetArgs::Option option[] =
    {
        {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
    };



    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    GlobalData &gdata = GlobalData::GetInstance();
    const GlobalData::DeviceList_t deviceList = gdata.GetDeviceList();

    data.SetStringListResult(GetKeys(deviceList));
}

/// Get the list of all the regions on the device currently loaded
/// There is only an error if there are no regions
/// Ultimately should be able to get based on material
void
getRegionListCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    dsGetArgs::Option *option;
    if (commandName == "get_region_list")
    {
      static dsGetArgs::Option getregionlistoption[] = {
          {"device", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
          {"contact", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr},
          {"interface", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr},
          {nullptr, nullptr,   dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
      };
      option = getregionlistoption;
    }
    else if (commandName == "get_interface_list" || commandName == "get_contact_list")
    {
      static dsGetArgs::Option getregionlistoption[] = {
          {"device", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
          {nullptr, nullptr,   dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
      };
      option = getregionlistoption;
    }
    else
    {
        /// This should not be able to happen
        dsAssert(false, "UNEXPECTED");
        option = nullptr;
    }





    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &deviceName = data.GetStringOption("device");
    Device *dev = nullptr;

    errorString = ValidateDevice(deviceName, dev);

    if (dev)
    {
        if (commandName == "get_region_list")
        {
          const std::string &interfaceName = data.GetStringOption("interface");
          const std::string &contactName = data.GetStringOption("contact");
          if ((!contactName.empty()) && (!interfaceName.empty()))
          {
            errorString += "Cannot specify both interface and contact\n";
            data.SetErrorResult(errorString);
            return;
          }
          else if (!contactName.empty())
          {
            Contact *contact = nullptr;
            errorString = ValidateDeviceAndContact(deviceName, contactName, dev, contact);
            if ((!errorString.empty()))
            {
              data.SetErrorResult(errorString);
              return;
            }
            if (contact)
            {
              const std::string &regionName = (contact->GetRegion())->GetName();
              std::vector<std::string> clist;
              clist.push_back(regionName);
              data.SetStringListResult(clist);
            }
          }
          else if (!interfaceName.empty())
          {
            Interface *interface = nullptr;
            errorString = ValidateDeviceAndInterface(deviceName, interfaceName, dev, interface);
            if ((!errorString.empty()))
            {
              data.SetErrorResult(errorString);
              return;
            }
            if (interface)
            {

              const std::string &regionName0 = (interface->GetRegion0())->GetName();
              const std::string &regionName1 = (interface->GetRegion1())->GetName();
              std::vector<std::string> rlist;
              rlist.push_back(regionName0);
              rlist.push_back(regionName1);
              data.SetStringListResult(rlist);
            }
          }
          else
          {
            const Device::RegionList_t &regionList = dev->GetRegionList();
            data.SetStringListResult(GetKeys(regionList));
          }
        }
        else if (commandName == "get_interface_list")
        {
            const Device::InterfaceList_t &interfaceList = dev->GetInterfaceList();
            data.SetStringListResult(GetKeys(interfaceList));
        }
        else if (commandName == "get_contact_list")
        {
            const Device::ContactList_t &contactList = dev->GetContactList();
            data.SetStringListResult(GetKeys(contactList));
        }
    }
}

namespace {
template <typename T>
void convertToObjectHolder(const T &eList, ObjectHolderList_t &olist, bool reorder)
{
  olist.reserve(eList.size());
  for (auto i : eList)
  {
    std::vector<ObjectHolder> x;
    x.reserve(i->GetNodeList().size());
    if (reorder)
    {
      for (auto j : i->GetFENodeList())
      {
        x.push_back(ObjectHolder(static_cast<int>(j->GetIndex())));
      }
    }
    else
    {
      for (auto j : i->GetNodeList())
      {
        x.push_back(ObjectHolder(static_cast<int>(j->GetIndex())));
      }
    }
    olist.push_back(ObjectHolder(x));
  }
}
}

void
getElementNodeListCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

      static dsGetArgs::Option option[] = {
      {"device", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
      {"region", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidRegion},
      {"contact", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr},
      {"interface", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr},
      {"reorder", "", dsGetArgs::optionType::BOOLEAN, dsGetArgs::requiredType::OPTIONAL, nullptr},
      {nullptr, nullptr,   dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
      };





    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &deviceName = data.GetStringOption("device");
    const bool reorder = data.GetBooleanOption("reorder");


    Device    *device = nullptr;
    Region    *region = nullptr;
    Contact   *contact = nullptr;
    Interface *interface = nullptr;

    ConstTetrahedronList tetrahedronList;
    ConstTriangleList    triangleList;
    ConstEdgeList        edgeList;
    ConstNodeList        nodeList;


    const std::string &regionName = data.GetStringOption("region");
    const std::string &contactName = data.GetStringOption("contact");
    const std::string &interfaceName = data.GetStringOption("interface");

    if ((!contactName.empty()) && (!interfaceName.empty()))
    {
      errorString += "Cannot specify both interface and contact\n";
      data.SetErrorResult(errorString);
      return;
    }

    if (!contactName.empty())
    {
      errorString = ValidateDeviceRegionAndContact(deviceName, regionName, contactName, device, region, contact);
      if (!errorString.empty())
      {
        data.SetErrorResult(errorString);
        return;
      }

      const size_t dimension = device->GetDimension();

      switch (dimension) {
        case 3:
          triangleList = contact->GetTriangles();
          break;
        case 2:
          edgeList     = contact->GetEdges();
          break;
        case 1:
          nodeList     = contact->GetNodes();
          break;
        default:
          dsAssert(false, "UNEXPECTED");
          break;
      };
    }
    else if (!interfaceName.empty())
    {
      errorString = ValidateDeviceRegionAndInterface(deviceName, regionName, interfaceName, device, region, interface);
      if (!errorString.empty())
      {
        data.SetErrorResult(errorString);
        return;
      }

      const size_t dimension = device->GetDimension();

      if (interface->GetRegion0() == region)
      {
        switch (dimension) {
          case 3:
            triangleList = interface->GetTriangles0();
            break;
          case 2:
            edgeList     = interface->GetEdges0();
            break;
          case 1:
            nodeList     = interface->GetNodes0();
            break;
          default:
            dsAssert(false, "UNEXPECTED");
            break;
        };
      }
      else if (interface->GetRegion1() == region)
      {
        switch (dimension) {
          case 3:
            triangleList = interface->GetTriangles1();
            break;
          case 2:
            edgeList     = interface->GetEdges1();
            break;
          case 1:
            nodeList     = interface->GetNodes1();
            break;
          default:
            dsAssert(false, "UNEXPECTED");
            break;
        };
      }
    }
    else if (!regionName.empty())
    {
      errorString = ValidateDeviceAndRegion(deviceName, regionName, device, region);
      if (!errorString.empty())
      {
        data.SetErrorResult(errorString);
        return;
      }

      const size_t dimension = device->GetDimension();

      switch (dimension) {
        case 3:
          tetrahedronList = region->GetTetrahedronList();
          break;
        case 2:
          triangleList    = region->GetTriangleList();
          break;
        case 1:
          edgeList        = region->GetEdgeList();
          break;
        default:
          dsAssert(false, "UNEXPECTED");
          break;
      };
    }


    ObjectHolderList_t olist;
    if (!tetrahedronList.empty())
    {
      convertToObjectHolder(tetrahedronList, olist, reorder);
    }
    else if (!triangleList.empty())
    {
      convertToObjectHolder(triangleList, olist, reorder);
    }
    else if (!edgeList.empty())
    {
      convertToObjectHolder(edgeList, olist, reorder);
    }
    else if (!nodeList.empty())
    {
      olist.reserve(nodeList.size());
      std::vector<ObjectHolder> x;
      for (auto i : nodeList)
      {
        x.push_back(ObjectHolder(static_cast<int>(i->GetIndex())));
      }
      olist.push_back(ObjectHolder(x));
    }
    data.SetObjectResult(ObjectHolder(olist));

}
}


