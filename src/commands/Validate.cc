/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "Validate.hh"
#include "Device.hh"
#include "Region.hh"
#include "GlobalData.hh"
#include "NodeSolution.hh"
#include "Interface.hh"
#include "Contact.hh"
#include <sstream>
namespace dsValidate {
std::string onRegiononDevice(const std::string &rnm, const std::string &dnm)
{
    std::ostringstream os;
    os << "on Region \"" << rnm << "\" on Device \"" << dnm << "\"";
    return os.str();
}

std::string onContactonDevice(const std::string &rnm, const std::string &dnm)
{
    std::ostringstream os;
    os << "on Contact \"" << rnm << "\" on Device \"" << dnm << "\"";
    return os.str();
}

std::string onInterfaceonDevice(const std::string &inm, const std::string &dnm)
{
    std::ostringstream os;
    os << "on Interface \"" << inm << "\" on Device \"" << dnm << "\"";
    return os.str();
}

std::string ValidateDevice(const std::string &deviceName, Device *&dev)
{
    dev = nullptr;
    std::string errorString;

    GlobalData &gdata = GlobalData::GetInstance();
    dev = gdata.GetDevice(deviceName);

    if (!dev)
    {
        std::ostringstream os;
        os << "Device \"" << deviceName << "\" does not exist.";
        errorString = os.str();
    }
    return errorString;

}

std::string ValidateDeviceAndRegion(const std::string &deviceName,
                                      const std::string &regionName,
                                      Device * &dev,
                                      Region * &reg)
{
    dev = nullptr;
    reg = nullptr;

    std::string errorString;

    errorString = ValidateDevice(deviceName, dev);

    if (dev)
    {
        reg = dev->GetRegion(regionName);
    }

    if (!reg)
    {
        std::ostringstream os;
        os << onRegiononDevice(regionName, deviceName) << " does not exist.\n";
        errorString = os.str();
    }

    return errorString;
}

std::string ValidateDeviceAndContact(const std::string &deviceName,
                                      const std::string &contactName,
                                      Device * &dev,
                                      Contact * &con)
{
    dev = nullptr;
    con = nullptr;

    std::string errorString;

    errorString = ValidateDevice(deviceName, dev);

    if (dev)
    {
        con = dev->GetContact(contactName);
    }

    if (!con)
    {
        std::ostringstream os;
        os << onContactonDevice(contactName, deviceName) << " does not exist.\n";
        errorString = os.str();
    }

    return errorString;
}


std::string ValidateDeviceRegionAndContact(const std::string &deviceName,
                                      const std::string &regionName,
                                      const std::string &contactName,
                                      Device * &dev,
                                      Region * &reg,
                                      Contact * &contact)
{
    contact = nullptr;
    std::string errorString = ValidateDeviceAndRegion(deviceName, regionName, dev, reg);
    if (!errorString.empty())
    {
        return errorString;
    }

    contact = dev->GetContact(contactName);
    if (!contact || (contact->GetRegion() != reg))
    {
        std::ostringstream os;
        os << "Contact \"" << contactName << "\" does not exist " << onRegiononDevice(deviceName, regionName) << "\n";
        errorString = os.str();
    }
    return errorString;
}


std::string ValidateDeviceAndInterface(const std::string &deviceName,
                                      const std::string &interfaceName,
                                      Device * &dev,
                                      Interface * &interface)
{
    dev = nullptr;
    interface = nullptr;

    std::string errorString;

    errorString = ValidateDevice(deviceName, dev);

    if (dev)
    {
        interface = dev->GetInterface(interfaceName);
    }

    if (!interface)
    {
        std::ostringstream os;
        os << "Interface \"" << interfaceName << "\" on Device \"" << deviceName << "\" does not exist.";
        errorString = os.str();
    }

    return errorString;
}

std::string ValidateDeviceRegionAndInterface(const std::string &deviceName,
                                      const std::string &regionName,
                                      const std::string &interfaceName,
                                      Device * &dev,
                                      Region * &region,
                                      Interface * &interface)
{
    dev = nullptr;
    interface = nullptr;

    std::string errorString;

    errorString = ValidateDeviceAndRegion(deviceName, regionName, dev, region);

    if (dev)
    {
        interface = dev->GetInterface(interfaceName);
    }

    if (!interface)
    {
        std::ostringstream os;
        os << "Interface \"" << interfaceName << "\" on Device \"" << deviceName << "\" does not exist.";
        errorString = os.str();
    }
    else if ((interface->GetRegion0() != region) && (interface->GetRegion1() != region))
    {
        std::ostringstream os;
        os << "Region \"" << regionName << "\" on Device \"" << deviceName << "\" on interface \"" << interfaceName << "\" does not exist.";
        errorString = os.str();
    }

    return errorString;
}


std::string ValidateNodeModelName(Device * const dev, Region * const reg, const std::string &node_model)
{
    std::string errorString;
    std::ostringstream os;

    if (node_model.empty())
    {
        os << "-node_model cannot be an empty string " << onRegiononDevice(reg->GetName(), dev->GetName()) << "\n";
        errorString = os.str();
    }
    else
    {
        ConstNodeModelPtr nm = reg->GetNodeModel(node_model);
        if (!nm)
        {
            os << "-node_model \"" << node_model << "\"" << onRegiononDevice(reg->GetName(), dev->GetName()) << " does not exist\n";
            errorString = os.str();
        }
    }

    return errorString;
}


std::string ValidateEdgeModelName(Device * const dev, Region * const reg, const std::string &edge_model)
{
    std::string errorString;
    std::ostringstream os;

    if (edge_model.empty())
    {
        os << "-edge_model cannot be an empty string on Region \"" << reg->GetName() <<
              "\" on Device \"" << dev->GetName() << "\"\n";
        errorString = os.str();
    }
    else
    {
        ConstEdgeModelPtr nm = reg->GetEdgeModel(edge_model);
        if (!nm)
        {
            os << "-edge_model \"" << edge_model << "\" on Region \"" << reg->GetName() <<
                  "\" on Device \"" << dev->GetName() << "\" does not exist\n";
            errorString = os.str();
        }
    }

    return errorString;
}

std::string ValidateOptionalNodeModelName(Device * const dev, Region * const reg, const std::string &node_model)
{
    std::string errorString;
    if (!node_model.empty())
    {
        errorString  = ValidateNodeModelName(dev, reg, node_model);
    }
    return errorString;
}

std::string ValidateOptionalEdgeModelName(Device * const dev, Region * const reg, const std::string &node_model)
{
    std::string errorString;
    if (!node_model.empty())
    {
        errorString  = ValidateEdgeModelName(dev, reg, node_model);
    }
    return errorString;
}

std::string ValidateInterfaceNodeModelName(Device * const dev, Interface * const interface, const std::string &interface_model)
{
    std::string errorString;
    std::ostringstream os;

    if (interface_model.empty())
    {
        os << "-interface_model cannot be an empty string " << onInterfaceonDevice(interface->GetName(), dev->GetName()) << "\n";
        errorString = os.str();
    }
    else
    {
        ConstInterfaceNodeModelPtr nm = interface->GetInterfaceNodeModel(interface_model);
        if (!nm)
        {
            os << "-interface_model \"" << interface_model << "\"" << onInterfaceonDevice(interface->GetName(), dev->GetName()) << " does not exist\n";
            errorString = os.str();
        }
    }

    return errorString;
}
}

