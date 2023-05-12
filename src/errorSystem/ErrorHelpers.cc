/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "ErrorHelpers.hh"
#include "GeneralErrors.hh"
#include "Device.hh"
#include "Region.hh"
#include "Contact.hh"
#include "Interface.hh"

namespace dsErrors {
std::string FormatDeviceAndRegionName(const Region &region)
{
  std::ostringstream os;
  os << "Device: " << region.GetDevice()->GetName() << " "
        "Region: " << region.GetName();
  return os.str();
}

std::string FormatEquationOnRegionName(const Region &region, const std::string &equation_name)
{
  std::ostringstream os;
  os << FormatDeviceAndRegionName(region)
     << " Equation: " << equation_name;
  return os.str();
}

std::string FormatModelNameAndType(const std::string &model_name, ModelInfo::ModelType model_type)
{
  std::ostringstream os;
  os << ModelInfo::ModelTypeStrings[model_type] << " Model: " << model_name;
  return os.str();
}

std::string FormatDeviceRegionAndContactName(const Region &region, const Contact &contact)
{
  std::ostringstream os;
  os << FormatDeviceAndRegionName(region)
     << " " <<
     " Contact: " << contact.GetName();
  return os.str();
}

std::string FormatDeviceRegionAndInterfaceName(const Region &region, const Interface &interface)
{
  std::ostringstream os;
  os << FormatDeviceAndRegionName(region)
     << " " <<
     " Interface: " << interface.GetName();
  return os.str();
}

std::string FormatContactEquationOnContactName(const Region &region, const Contact &contact, const std::string &equation_name)
{
  std::ostringstream os;
  os << FormatDeviceRegionAndContactName(region, contact)
     << " Contact Equation: " << equation_name;
  return os.str();
}

std::string FormatInterfaceEquationOnInterfaceName(const Region &region, const Interface &interface, const std::string &equation_name)
{
  std::ostringstream os;
  os << FormatDeviceRegionAndInterfaceName(region, interface)
     << " Interface Equation: " << equation_name;
  return os.str();
}
}

