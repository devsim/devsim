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

#include "ErrorHelpers.hh"
#include "Contact.hh"
#include "Device.hh"
#include "GeneralErrors.hh"
#include "Interface.hh"
#include "Region.hh"

namespace dsErrors {
std::string FormatDeviceAndRegionName(const Region &region) {
  std::ostringstream os;
  os << "Device: " << region.GetDevice()->GetName()
     << " "
        "Region: "
     << region.GetName();
  return os.str();
}

std::string FormatEquationOnRegionName(const Region &region,
                                       const std::string &equation_name) {
  std::ostringstream os;
  os << FormatDeviceAndRegionName(region) << " Equation: " << equation_name;
  return os.str();
}

std::string FormatModelNameAndType(const std::string &model_name,
                                   ModelInfo::ModelType model_type) {
  std::ostringstream os;
  os << ModelInfo::ModelTypeStrings[model_type] << " Model: " << model_name;
  return os.str();
}

std::string FormatDeviceRegionAndContactName(const Region &region,
                                             const Contact &contact) {
  std::ostringstream os;
  os << FormatDeviceAndRegionName(region) << " "
     << " Contact: " << contact.GetName();
  return os.str();
}

std::string FormatDeviceRegionAndInterfaceName(const Region &region,
                                               const Interface &interface) {
  std::ostringstream os;
  os << FormatDeviceAndRegionName(region) << " "
     << " Interface: " << interface.GetName();
  return os.str();
}

std::string
FormatContactEquationOnContactName(const Region &region, const Contact &contact,
                                   const std::string &equation_name) {
  std::ostringstream os;
  os << FormatDeviceRegionAndContactName(region, contact)
     << " Contact Equation: " << equation_name;
  return os.str();
}

std::string
FormatInterfaceEquationOnInterfaceName(const Region &region,
                                       const Interface &interface,
                                       const std::string &equation_name) {
  std::ostringstream os;
  os << FormatDeviceRegionAndInterfaceName(region, interface)
     << " Interface Equation: " << equation_name;
  return os.str();
}
} // namespace dsErrors
