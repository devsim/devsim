/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef GEOMETRYCOMMANDS_HH
#define GEOMETRYCOMMANDS_HH
class CommandHandler;
namespace dsCommand {
struct Commands;
extern Commands GeometryCommands[];
void resetDevsimCmd(CommandHandler &);
void getDeviceListCmd(CommandHandler &);
void getRegionListCmd(CommandHandler &);
void getElementNodeListCmd(CommandHandler &);
}

#endif

