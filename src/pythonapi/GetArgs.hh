/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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
  CommandInfo() : command_handler_(nullptr), get_args_(nullptr), self_(nullptr), args_(nullptr), kwargs_(nullptr), exception_(nullptr)
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
    GetArgs(optionList opts);

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
        ObjectHolderMap_t selections;
};
}
#endif

