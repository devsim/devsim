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

#ifndef MESHINGCOMMANDS_HH
#define MESHINGCOMMANDS_HH
class CommandHandler;
namespace dsCommand {
struct Commands;
extern Commands MeshingCommands[];
void add1dContactCmd(CommandHandler &);
void add1dInterfaceCmd(CommandHandler &);
void add1dMeshLineCmd(CommandHandler &);
void add1dRegionCmd(CommandHandler &);
void add2dContactCmd(CommandHandler &);
void add2dInterfaceCmd(CommandHandler &);
void add2dMeshLineCmd(CommandHandler &);
void add2dRegionCmd(CommandHandler &);
void addGmshContactCmd(CommandHandler &);
void addGmshInterfaceCmd(CommandHandler &);
void addGmshRegionCmd(CommandHandler &);
void create1dMeshCmd(CommandHandler &);
void createContactFromInterfaceCmd(CommandHandler &);
void createDeviceCmd(CommandHandler &);
void createGmshMeshCmd(CommandHandler &);
void finalizeMeshCmd(CommandHandler &);
void loadDevicesCmd(CommandHandler &);
void writeDevicesCmd(CommandHandler &);
}

#endif

