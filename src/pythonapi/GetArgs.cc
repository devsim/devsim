/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "GetArgs.hh"
#include "CommandHandler.hh"
#include "dsAssert.hh"
#include <sstream>
#include <limits>

#include "GetArgsCommon.cc"

namespace dsGetArgs {
bool GetArgs::processOptions(CommandInfo &tdata, std::string &error)
{
  bool status = false;

  CommandHandler &handler = *tdata.command_handler_;

  ObjectHolder hash(tdata.kwargs_);
  ObjectHolderMap_t hashmap;
  hash.GetHashMap(hashmap);

  const std::string &commandName = handler.GetCommandName();

  for (ObjectHolderMap_t::iterator it = hashmap.begin(); it != hashmap.end();
       ++it)
  {
    const std::string &optname = it->first;
    if (!optname.empty())
    {
      if (optionMap.count(optname))
      {
        Option *opt = optionMap[optname];
        optionType type = (*opt).type;

        if (selections.count(optname))
        {
          alreadySpecified(commandName, optname, selections[optname], error);
          status = true;
          break;
        }
        else
        {
          ObjectHolder value = it->second;
          selections[optname] = value;

          const ObjectHolder &toh = selections[optname];
          if (type == optionType::FLOAT)
          {
            ObjectHolder::DoubleEntry_t ret = toh.GetDouble();
            if (!ret.first)
            {
              notConvertibleToType(commandName, optname, optionType::FLOAT, toh,
                                   error);
              status = true;
              break;
            }
          }
          else if (type == optionType::BOOLEAN)
          {
            ObjectHolder::BooleanEntry_t ret = toh.GetBoolean();
            if (!ret.first)
            {
              notConvertibleToType(commandName, optname, optionType::BOOLEAN,
                                   toh, error);
              status = true;
              break;
            }
          }
          else if (type == optionType::INTEGER)
          {
            ObjectHolder::IntegerEntry_t ret = toh.GetInteger();
            if (!ret.first)
            {
              notConvertibleToType(commandName, optname, optionType::INTEGER,
                                   toh, error);
              status = true;
              break;
            }
          }
          else if (type == optionType::LIST)
          {
            bool ret = toh.IsList();
            if (!ret)
            {
              notConvertibleToType(commandName, optname, optionType::LIST, toh,
                                   error);
              status = true;
              break;
            }
          }
          else if (type == optionType::STRING)
          {
          }
          else
          {
            dsAssert(true, "UNEXPECTED");
          }
        }
      }
      else
      {
        error = optname;
        error += " is an invalid option\n";
        status = true;
        break;
      }
    }
  }  // end for loop through arguments

  //// Check for required parameters
  if (!status)
  {
    std::ostringstream os;
    Option *it = options;
    while ((*it).name != nullptr)
    {
      std::string name = (*it).name;
      if ((*it).rtype == requiredType::REQUIRED)
      {
        if (!selections.count(name))
        {
          os << "missing required "
             << optionTypeStrings[static_cast<size_t>((*it).type)]
             << " parameter - " << name << "\n";
          status = true;
        }
      }
      ++it;
    }

    if (status == true)
    {
      error = os.str();
    }
  }

  ///// Call the callbackfunctions
  if (!status)
  {
    std::ostringstream os;
    std::string errorString;
    Option *it = options;
    while ((*it).name != nullptr)
    {
      if ((*it).func)
      {
        if (!(((*it).func)((*it).name, errorString, handler)))
        {
          os << errorString;
          status = true;
        }
      }
      ++it;
    }

    if (status == true)
    {
      error = os.str();
    }
  }

  return status;
}
}  // namespace dsGetArgs
