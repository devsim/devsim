/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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
void createInterfaceFromNodesCmd(CommandHandler &);
void createDeviceCmd(CommandHandler &);
void createGmshMeshCmd(CommandHandler &);
void deleteDeviceCmd(CommandHandler &);
void deleteMeshCmd(CommandHandler &);
void finalizeMeshCmd(CommandHandler &);
void loadDevicesCmd(CommandHandler &);
void writeDevicesCmd(CommandHandler &);
void getMeshListCmd(CommandHandler &);
}

#endif

