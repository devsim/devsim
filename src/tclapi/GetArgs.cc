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
#include <tcl.h>
//// Make sure this is synced up with the python version
namespace dsGetArgs {
const char * Types::optionTypeStrings[] = {"BOOLEAN", "STRING", "INTEGER", "FLOAT"};

GetArgs::GetArgs(optionList opts, switchList sl) : options(opts), switches(sl)
{
  Option *it = options;
  while ((*it).name != NULL)
  {
    optionMap[(*it).name] = it;
    ++it;
  }
} 

ObjectHolder GetArgs::GetObjectHolder(const std::string &s) const
{
  ObjectHolder val;
  ObjectHolderMap_t::const_iterator it = selections.find(s);
  if (it != selections.end())
  {
    val = (it->second);
  }
  else
  {
    OptionMap_t::const_iterator vit = optionMap.find(s);
    dsAssert(vit != optionMap.end(), "UNEXPECTED");
    const std::string &sval = (vit->second)->defaultValue;
    val = ObjectHolder(sval);
  }
  return val;
}

std::string GetArgs::GetStringOption(const std::string &s) const
{
  const ObjectHolder &tobj = GetObjectHolder(s);
  return tobj.GetString();
}

double GetArgs::GetDoubleOption(const std::string &s) const
{
  double val = 0.0;

  const ObjectHolder &tobj = GetObjectHolder(s);

  ObjectHolder::DoubleEntry_t ret = tobj.GetDouble();

  if (ret.first)
  {
    val = ret.second;
  }
  else
  {
    const std::string &sval = tobj.GetString();

    if (sval == "MAXDOUBLE")
    {
      val = std::numeric_limits<double>::max();
    }
    else if (sval == "-MAXDOUBLE")
    {
      val = -std::numeric_limits<double>::max();
    }
  }
  return val;
}

bool GetArgs::GetBooleanOption(const std::string &s) const
{
  bool val = false;

  const ObjectHolder &tobj = GetObjectHolder(s);

  ObjectHolder::BooleanEntry_t ret = tobj.GetBoolean();

  if (ret.first)
  {
    val = ret.second;
  }

  return val;
}

int GetArgs::GetIntegerOption(const std::string &s) const
{
  int val = 0;

  const ObjectHolder &tobj = GetObjectHolder(s);

  ObjectHolder::IntegerEntry_t ret = tobj.GetInteger();

  if (ret.first)
  {
    val = ret.second;
  }

  return val;
}

namespace {
void alreadySpecified(const std::string &command, const std::string &arg, const ObjectHolder &val, std::string &error)
{
  std::ostringstream os;
  os << command << " already passed option " << arg;
  os << " with value " << val.GetString() << " as an option" << "\n";
  error = os.str();
}
#if 0
void alreadySpecified(const std::string &command, const std::string &arg, const double val, std::string &error)
{
  std::ostringstream os;
  os << command << " already passed option " << arg << " with value " << val << " as an option" << "\n";
  error = os.str();
}
#endif

void notSpecified(const std::string &command, const std::string &arg, std::string &error)
{
  std::ostringstream os;
  os << command << " does not specify a value for option " << arg << "\n";
  error = os.str();
}

void notConvertibleToType(const std::string &command, const std::string &arg, Types::optionType ot, const ObjectHolder &val, std::string &error)
{
  std::ostringstream os;
  os << "Cannot convert \"" << val.GetString() << "\" to a " << Types::optionTypeStrings[ot] <<  " for argument " << arg;

  error = os.str();
}
}

bool GetArgs::processOptions(CommandInfo &tdata, std::string &error)
{
  int status = TCL_OK; // This is ok

  CommandHandler &handler = *tdata.command_handler_;
  Tcl_Obj *const* objv = tdata.objv_;
  const int objc = tdata.objc_;
  Tcl_Interp *interp = tdata.interp_;

  std::string commandName = Tcl_GetStringFromObj(objv[0], NULL);

  for (int i = 1; i < objc; ++i)
  {
    std::string optname = Tcl_GetStringFromObj(objv[i], NULL);
    if ((optname.size() > 0) && (optname[0] == '-'))
    {
      /// Get the stuff past the dash
      optname = optname.substr(1);
      if (optionMap.count(optname))
      {
        Option     *opt  = optionMap[optname];
        Types::optionType  type = (*opt).type;  

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
            if (type == Types::FLOAT)
            {
              ObjectHolder::DoubleEntry_t ret = toh.GetDouble();
              if (!ret.first)
              {
                notConvertibleToType(commandName, optname, Types::FLOAT, toh, error);
                status = TCL_ERROR;
                break;
              }
            }
            else if (type == Types::BOOLEAN)
            {
              ObjectHolder::BooleanEntry_t ret = toh.GetBoolean();
              if (!ret.first)
              {
                notConvertibleToType(commandName, optname, Types::BOOLEAN, toh, error);
                status = TCL_ERROR;
                break;
              }
            }
            else if (type == Types::INTEGER)
            {
              ObjectHolder::IntegerEntry_t ret = toh.GetInteger();
              if (!ret.first)
              {
                notConvertibleToType(commandName, optname, Types::INTEGER, toh, error);
                status = TCL_ERROR;
                break;
              }
            }
            else if (type == Types::STRING)
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
    while ((*it).name != NULL)
    {
      std::string name = (*it).name;
      if ((*it).rtype == Types::REQUIRED)
      {
        if (!selections.count(name))
        {
            os << "missing required " << Types::optionTypeStrings[(*it).type] <<  " parameter - " << name << "\n";
            status = TCL_ERROR;
        }
      }
      ++it;
    }

    if (status == TCL_ERROR)
    {
        error = os.str();
        Tcl_SetErrorCode(interp, error.c_str(), NULL);
    }
  }

  ///// Call the callbackfunctions
  if (status == TCL_OK)
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
// TODO: "listobj processing"
