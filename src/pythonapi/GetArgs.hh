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

#ifndef GETARGS_HH
#define GETARGS_HH
#include "Python.h"
#include "ObjectHolder.hh"
#include "CommandHandler.hh"
#include <set>
#include <string>
#include <map>


class CommandHandler;
namespace dsGetArgs {

class GetArgs;

struct CommandInfo
{
  CommandInfo() : command_handler_(NULL), get_args_(NULL), self_(NULL), args_(NULL), kwargs_(NULL), exception_(NULL)
  {
  }

  CommandHandler *command_handler_;
  GetArgs *get_args_;
  PyObject *self_;
  PyObject *args_;
  PyObject *kwargs_;
  PyObject *exception_;
  std::string command_name_;
};


class GetArgs {
    public:
    /// Could only add one options
    GetArgs(optionList opts, switchList sl);

    /// False throws an exception with our error message
//    int processOptions(TclCommandHandler &, std::string &error);

    bool processOptions(CommandInfo &, std::string &);

    std::string GetStringOption(const std::string &) const;
    double GetDoubleOption(const std::string &) const;
    int    GetIntegerOption(const std::string &) const;
    bool   GetBooleanOption(const std::string &) const;

    ObjectHolder GetObjectHolder(const std::string &) const;

    typedef std::map<std::string, ObjectHolder> ObjectHolderMap_t;
    typedef std::map<std::string, Option *>     OptionMap_t;
    private:
        OptionMap_t       optionMap;
        optionList        options;
        switchList        switches;
        ObjectHolderMap_t selections;
};
}
#endif

