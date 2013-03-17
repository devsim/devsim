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
