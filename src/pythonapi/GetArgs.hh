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

