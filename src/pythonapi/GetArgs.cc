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
#if 0
  Tcl_Obj *const* objv = tdata.objv_;
  const int objc = tdata.objc_;
  Tcl_Interp *interp = tdata.interp_;
#endif

  ObjectHolder hash(tdata.kwargs_);
  ObjectHolderMap_t hashmap;
  hash.GetHashMap(hashmap);
  

  const std::string &commandName = handler.GetCommandName();

  for (ObjectHolderMap_t::iterator it = hashmap.begin(); it != hashmap.end(); ++it)
  {
    const std::string &optname = it->first;
    if (!optname.empty())
    {
      /// Get the stuff past the dash
      if (optionMap.count(optname))
      {
        Option     *opt  = optionMap[optname];
        Types::optionType  type = (*opt).type;  

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
          if (type == Types::FLOAT)
          {
            ObjectHolder::DoubleEntry_t ret = toh.GetDouble();
            if (!ret.first)
            {
              notConvertibleToType(commandName, optname, Types::FLOAT, toh, error);
              status = true;
              break;
            }
          }
          else if (type == Types::BOOLEAN)
          {
            ObjectHolder::BooleanEntry_t ret = toh.GetBoolean();
            if (!ret.first)
            {
              notConvertibleToType(commandName, optname, Types::BOOLEAN, toh, error);
              status = true;
              break;
            }
          }
          else if (type == Types::INTEGER)
          {
            ObjectHolder::IntegerEntry_t ret = toh.GetInteger();
            if (!ret.first)
            {
              notConvertibleToType(commandName, optname, Types::INTEGER, toh, error);
              status = true;
              break;
            }
          }
          else if (type == Types::LIST)
          {
            bool ret = toh.IsList();
            if (!ret)
            {
              notConvertibleToType(commandName, optname, Types::LIST, toh, error);
              status = true;
              break;
            }
          }
          else if (type == Types::STRING)
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
  }//end for loop through arguments

  //// Check for required parameters
  if (!status)
  {
    std::ostringstream os;
    Option *it = options;
    while ((*it).name != NULL)
    {
      std::string name = (*it).name;
      if ((*it).rtype == Types::REQUIRED)
      {
        if (!selections.count(name))
        {
            os << "missing required " << Types::optionTypeStrings[(*it).type] <<  " parameter - " << name << "\n";
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
    while ((*it).name != NULL)
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
}

