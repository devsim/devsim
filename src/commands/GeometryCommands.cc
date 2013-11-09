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

#include <sstream>

using namespace dsValidate;

namespace dsCommand {
/// Get the list of all the devices currently loaded
/// There is only an error if there are no devices
void getDeviceListCmd(CommandHandler &data)
{
    std::string errorString;

    /// Will need someway of setting circuit node
    /// (This would be on the contact and not the contact equation??)
    static dsGetArgs::Option option[] =
    {
        {NULL,  NULL, dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL}
    };

    dsGetArgs::switchList switches = NULL;


    bool error = data.processOptions(option, switches, errorString);

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
          {"device", "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, mustBeValidDevice},
          {"contact", "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL},
          {"interface", "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL},
          {NULL, NULL,   dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL}
      };
      option = getregionlistoption;
    }
    else if (commandName == "get_interface_list" || commandName == "get_contact_list")
    {
      static dsGetArgs::Option getregionlistoption[] = {
          {"device", "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, mustBeValidDevice},
          {NULL, NULL,   dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL}
      };
      option = getregionlistoption;
    }
    else
    {
        /// This should not be able to happen
        dsAssert(false, "UNEXPECTED");
        option = NULL;
    }



    dsGetArgs::switchList switches = NULL;


    bool error = data.processOptions(option, switches, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &deviceName = data.GetStringOption("device");
    Device *dev = NULL;

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
            Contact *contact = NULL;
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
            Interface *interface = NULL;
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

Commands GeometryCommands[] = {
    {"get_device_list",             getDeviceListCmd},
    {"get_region_list",             getRegionListCmd},
    {"get_interface_list",          getRegionListCmd},
    {"get_contact_list",            getRegionListCmd},
    {NULL, NULL}
};

}

//TODO: get_interface_node_indexes <two lists with r0, r1>
//TODO: get_contact_node_indexes
//TODO: get_edge_node_indexes <two lists with n0, n1>
//TODO: get_node_edge_indexes <two lists with e0, eq1 .... eqN>
//TODO: find closest node
//TODO: find closest edge
//TODO: find closest node

