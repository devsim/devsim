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
