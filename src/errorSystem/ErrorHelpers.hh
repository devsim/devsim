/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef DS_ERROR_HELPERS_HH
#define DS_ERROR_HELPERS_HH

#include "GeneralErrors.hh"

class Region;
class Contact;
class Interface;
#include <string>

namespace dsErrors {
std::string FormatDeviceAndRegionName(const Region &/*region*/);
std::string FormatEquationOnRegionName(const Region &/*region*/, const std::string &/*equation_name*/);
std::string FormatModelNameAndType(const std::string &/*model_name*/, ModelInfo::ModelType /*model_type*/);
std::string FormatDeviceRegionAndContactName(const Region &/*region*/, const Contact &/*contact*/);
std::string FormatDeviceRegionAndInterfaceName(const Region &/*region*/, const Interface &/*interface*/);
std::string FormatContactEquationOnContactName(const Region &/*region*/, const Contact &/*contact*/, const std::string &/*equation_name*/);
std::string FormatInterfaceEquationOnInterfaceName(const Region &/*region*/, const Interface &/*interface*/, const std::string &/*equation_name*/);
}
#endif
