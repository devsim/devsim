/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef DS_COMMANDS_HH
#define DS_COMMANDS_HH
class CommandHandler;
typedef void (*newcmd)(CommandHandler &);

namespace dsCommand {
struct Commands
{
  const char *name;
  newcmd      command;
};
}
#endif
