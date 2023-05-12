/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef MATHCOMMANDS_HH
#define MATHCOMMANDS_HH
class CommandHandler;
namespace dsCommand {
struct Commands;
extern Commands MathCommands[];
void getContactCurrentCmd(CommandHandler &);
void getContactCurrentCmd(CommandHandler &);
void solveCmd(CommandHandler &);
void getMatrixAndRHSCmd(CommandHandler &);
void setInitialConditionCmd(CommandHandler &);
}

#endif
