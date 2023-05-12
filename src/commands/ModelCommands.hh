/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef MODELCOMMANDS_HH
#define MODELCOMMANDS_HH
class CommandHandler;
namespace dsCommand {
struct Commands;
extern Commands ModelCommands[];
void createContactNodeModelCmd(CommandHandler &);
void createCylindricalCmd(CommandHandler &);
void createEdgeAverageModelCmd(CommandHandler &);
void createEdgeFromNodeModelCmd(CommandHandler &);
void createInterfaceNodeModelCmd(CommandHandler &);
void createInterfaceNormalModelCmd(CommandHandler &);
void createNodeModelCmd(CommandHandler &);
void createNodeSolutionCmd(CommandHandler &);
void createTriangleFromEdgeModelCmd(CommandHandler &);
void createVectorElementModelCmd(CommandHandler &);
void debugTriangleCmd(CommandHandler &);
void getInterfaceModelListCmd(CommandHandler &);
void getInterfaceValuesCmd(CommandHandler &);
void getNodeModelListCmd(CommandHandler &);
void printEdgeValuesCmd(CommandHandler &);
void printElementEdgeValuesCmd(CommandHandler &);
void printNodeValuesCmd(CommandHandler &);
void registerFunctionCmd(CommandHandler &);
void setNodeValueCmd(CommandHandler &);
void setNodeValuesCmd(CommandHandler &);
void symdiffCmd(CommandHandler &);
}

#endif
