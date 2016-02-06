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
