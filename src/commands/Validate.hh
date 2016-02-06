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

#ifndef VALIDATE_HH
#define VALIDATE_HH
#include <string>
class Device;
class Region;
class Interface;
class Contact;
namespace dsValidate {
std::string onRegiononDevice(const std::string &/*rnm*/, const std::string &/*dnm*/);
std::string onContactonDevice(const std::string &/*rnm*/, const std::string &/*dnm*/);

std::string onInterfaceonDevice(const std::string &/*inm*/, const std::string &/*dnm*/);

std::string ValidateDevice(const std::string &/*deviceName*/, Device *&/*dev*/);

std::string ValidateDeviceAndRegion(const std::string &/*deviceName*/,
                                      const std::string &/*regionName*/,
                                      Device * &/*dev*/,
                                      Region * &/*reg*/);

std::string ValidateDeviceAndContact(const std::string &/*deviceName*/,
                                      const std::string &/*regionName*/,
                                      Device * &/*dev*/,
                                      Contact * &/*cp*/);

#if 0
//// schedule for removal
std::string ValidateDeviceAndRegionAndContact(const std::string &/*deviceName*/,
                                      const std::string &/*regionName*/,
                                      const std::string &/*contactName*/,
                                      Device * &/*dev*/,
                                      Region * &/*reg*/,
                                      Contact * &/*contact*/);
#endif



std::string ValidateDeviceAndInterface(const std::string &/*deviceName*/,
                                      const std::string &/*interfaceName*/,
                                      Device * &/*dev*/,
                                      Interface * &/*interface*/);

std::string ValidateDeviceRegionAndInterface(const std::string &/*deviceName*/,
                                      const std::string &/*regionName*/,
                                      const std::string &/*interfaceName*/,
                                      Device * &/*dev*/,
                                      Region * &/*region*/,
                                      Interface * &/*interface*/);

std::string ValidateNodeModelName(Device * const /*dev*/, Region * const /*reg*/, const std::string &/*node_model*/);

std::string ValidateEdgeModelName(Device * const /*dev*/, Region * const /*reg*/, const std::string &/*edge_model*/);

std::string ValidateOptionalNodeModelName(Device * const /*dev*/, Region * const /*reg*/, const std::string &/*node_model*/);

std::string ValidateOptionalEdgeModelName(Device * const /*dev*/, Region * const /*reg*/, const std::string &/*node_model*/);

std::string ValidateInterfaceNodeModelName(Device * const /*dev*/, Interface * const /*interface*/, const std::string &/*interface_model*/);
}
#endif
