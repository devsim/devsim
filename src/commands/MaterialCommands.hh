/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef MATERIALCOMMANDS_HH
#define MATERIALCOMMANDS_HH
class CommandHandler;
namespace dsCommand {
struct Commands;
extern Commands MaterialCommands[];
void addDBEntryCmd(CommandHandler &);
void getDBEntryCmd(CommandHandler &);
void getParameterCmd(CommandHandler &);
void openDBCmd(CommandHandler &);
void MaterialCommandMissing(CommandHandler &);
}

#endif
