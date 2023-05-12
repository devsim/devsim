/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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
                                      const std::string &/*contactName*/,
                                      Device * &/*dev*/,
                                      Contact * &/*cp*/);

std::string ValidateDeviceRegionAndContact(const std::string &/*deviceName*/,
                                      const std::string &/*regionName*/,
                                      const std::string &/*contactName*/,
                                      Device * &/*dev*/,
                                      Region * &/*reg*/,
                                      Contact * &/*contact*/);



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
