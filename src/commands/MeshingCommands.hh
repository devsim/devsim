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
void addGeniusContactCmd(CommandHandler &);
void addGeniusInterfaceCmd(CommandHandler &);
void addGeniusRegionCmd(CommandHandler &);
void addGmshContactCmd(CommandHandler &);
void addGmshInterfaceCmd(CommandHandler &);
void addGmshRegionCmd(CommandHandler &);
void create1dMeshCmd(CommandHandler &);
void createContactFromInterfaceCmd(CommandHandler &);
void createDeviceCmd(CommandHandler &);
void createGeniusMeshCmd(CommandHandler &);
void createGmshMeshCmd(CommandHandler &);
void finalizeMeshCmd(CommandHandler &);
void loadDevicesCmd(CommandHandler &);
void writeDevicesCmd(CommandHandler &);
}

#endif

