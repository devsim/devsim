/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef CIRCUITCOMMANDS_HH
#define CIRCUITCOMMANDS_HH
class CommandHandler;
namespace dsCommand {
struct Commands;
extern Commands CircuitCommands[];
void addCircuitNodeCmd(CommandHandler &);
void circuitAlterCmd(CommandHandler &);
void circuitDeleteCircuitCmd(CommandHandler &);
void circuitElementCmd(CommandHandler &);
void circuitGetCircuitEquationNumberCmd(CommandHandler &);
void circuitGetCircuitNodeListCmd(CommandHandler &);
void circuitGetCircuitNodeValueCmd(CommandHandler &);
void circuitGetCircuitSolutionListCmd(CommandHandler &);
void circuitNodeAliasCmd(CommandHandler &);
}
#endif

