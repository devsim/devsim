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

#include "GetArgs.hh"
#include "CommandHandler.hh"
#include "dsAssert.hh"
#include <sstream>
#include <limits>
#include <tcl.h>
#include "GetArgsCommon.cc"

namespace dsGetArgs {
bool GetArgs::processOptions(CommandInfo &tdata, std::string &error)
{
  int status = TCL_OK; // This is ok

  CommandHandler &handler = *tdata.command_handler_;
  Tcl_Obj *const* objv = tdata.objv_;
  const int objc = tdata.objc_;
  Tcl_Interp *interp = tdata.interp_;

  const std::string &commandName = handler.GetCommandName();

  for (int i = 1; i < objc; ++i)
  {
    std::string optname = Tcl_GetStringFromObj(objv[i], nullptr);
    if ((optname.size() > 0) && (optname[0] == '-'))
    {
      /// Get the stuff past the dash
      optname = optname.substr(1);
      if (optionMap.count(optname))
      {
        Option     *opt  = optionMap[optname];
        optionType  type = (*opt).type;  

        if (selections.count(optname))
        {
          alreadySpecified(commandName, optname, selections[optname], error);
          status = TCL_ERROR;
          break;
        }
        else
        {
          ++i;
          if ( i < objc )
          {
            selections[optname] = ObjectHolder(objv[i]);

            const ObjectHolder &toh = selections[optname];
            if (type == optionType::FLOAT)
            {
              ObjectHolder::DoubleEntry_t ret = toh.GetDouble();
              if (!ret.first)
              {
                notConvertibleToType(commandName, optname, optionType::FLOAT, toh, error);
                status = TCL_ERROR;
                break;
              }
            }
            else if (type == optionType::BOOLEAN)
            {
              ObjectHolder::BooleanEntry_t ret = toh.GetBoolean();
              if (!ret.first)
              {
                notConvertibleToType(commandName, optname, optionType::BOOLEAN, toh, error);
                status = TCL_ERROR;
                break;
              }
            }
            else if (type == optionType::INTEGER)
            {
              ObjectHolder::IntegerEntry_t ret = toh.GetInteger();
              if (!ret.first)
              {
                notConvertibleToType(commandName, optname, optionType::INTEGER, toh, error);
                status = TCL_ERROR;
                break;
              }
            }
            else if (type == optionType::LIST)
            {
              bool ret = toh.IsList();
              if (!ret)
              {
                notConvertibleToType(commandName, optname, optionType::LIST, toh, error);
                status = TCL_ERROR;
                break;
              }
            }
            else if (type == optionType::STRING)
            {
            }
            else
            {
              dsAssert(false, "UNEXPECTED");
            }
          }
          else
          {
            notSpecified(commandName, optname, error);
            status = TCL_ERROR;
            break;
          }
        }
      }
      else
      {
        error = "-";
        error += optname;
        error += " is an invalid option\n";
        status = TCL_ERROR;
        break;
      }
    }
  }//end for loop through arguments

  //// Check for required parameters
  if (status == TCL_OK)
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
            os << "missing required " << optionTypeStrings[static_cast<size_t>((*it).type)] <<  " parameter - " << name << "\n";
            status = TCL_ERROR;
        }
      }
      ++it;
    }

    if (status == TCL_ERROR)
    {
        error = os.str();
        Tcl_SetErrorCode(interp, error.c_str(), nullptr);
    }
  }

  ///// Call the callbackfunctions
  if (status == TCL_OK)
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
          status = TCL_ERROR;
        }
      }
      ++it;
    }

    if (status == TCL_ERROR)
    {
      error = os.str();
    }
  }

  return status;
}
}

