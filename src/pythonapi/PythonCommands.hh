/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef DS_PYTHON_COMMANDS_HH
#define DS_PYTHON_COMMANDS_HH
#include "Python.h"
class CommandHandler;
typedef void (*newcmd)(CommandHandler &);
namespace dsPy {
bool Commands_Init();
}
#endif

