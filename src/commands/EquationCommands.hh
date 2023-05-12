/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef EQUATIONCOMMANDS_HH
#define EQUATIONCOMMANDS_HH
class CommandHandler;
namespace dsCommand {
struct Commands;
extern Commands EquationCommands[];
void createEquationCmd(CommandHandler &);
void createInterfaceEquationCmd(CommandHandler &);
void createContactEquationCmd(CommandHandler &);
void createCustomEquationCmd(CommandHandler &);
void getEquationNumbersCmd(CommandHandler &);
void getEquationListCmd(CommandHandler &);
void getInterfaceEquationListCmd(CommandHandler &);
void getContactEquationListCmd(CommandHandler &);
void deleteEquationCmd(CommandHandler &);
void deleteInterfaceEquationCmd(CommandHandler &);
void deleteContactEquationCmd(CommandHandler &);
void deleteEquationCmd(CommandHandler &);
void deleteContactEquationCmd(CommandHandler &);
void deleteInterfaceEquationCmd(CommandHandler &);
}

#endif
