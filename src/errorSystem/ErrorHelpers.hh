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
