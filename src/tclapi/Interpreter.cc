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


#include "Interpreter.hh"
#include "dsAssert.hh"
#include "GetGlobalParameter.hh"
#include <tcl.h>

//#include <iostream>

Interpreter::Interpreter() //: interp_(NULL)
{  
}

Interpreter::~Interpreter()
{
}


/* for now run in main intepreter*/
bool Interpreter::RunCommand(const std::string &command, const std::vector<std::string> &arguments)
{
  Tcl_Interp *interp_ = reinterpret_cast<Tcl_Interp *>(GetMainInterpreter());
  ObjectHolder tobj_holder(command);
  Tcl_Obj *tobj = reinterpret_cast<Tcl_Obj *>(tobj_holder.GetObject());
  for (std::vector<std::string>::const_iterator it = arguments.begin(); it != arguments.end(); ++it) 
  {
    Tcl_ListObjAppendElement(interp_, tobj, Tcl_NewStringObj(it->c_str(), -1));
  }
  int ret = Tcl_EvalObjEx(interp_, tobj, TCL_EVAL_GLOBAL);
  if (ret == TCL_OK)
  {
    error_string_.clear();
    result_ = ObjectHolder(Tcl_GetObjResult(interp_));
  }
  else
  {
    error_string_ = Tcl_GetStringResult(interp_);
    result_ = ObjectHolder();
  }
  Tcl_ResetResult(interp_);
  return (ret == TCL_OK);
}

bool Interpreter::RunCommand(std::vector<ObjectHolder> &objects)
{
//// TODO: make sure this is valid to do when the ObjectHolder has one data member
  Tcl_Interp *interp_ = reinterpret_cast<Tcl_Interp *>(GetMainInterpreter());
  int ret = Tcl_EvalObjv(interp_, objects.size(), (Tcl_Obj **) &objects[0], TCL_EVAL_GLOBAL); 
  if (ret == TCL_OK)
  {
    error_string_.clear();
    result_ = ObjectHolder(Tcl_GetObjResult(interp_));
  }
  else
  {
    error_string_ = Tcl_GetStringResult(interp_);
    result_ = ObjectHolder();
  }
  Tcl_ResetResult(interp_);
  return (ret == TCL_OK);
}

bool Interpreter::RunInternalCommand(const std::string &command, const std::vector<std::pair<std::string, ObjectHolder> > &arguments)
{
  return RunCommand(command, arguments);
}

bool Interpreter::RunCommand(const std::string &command, const std::vector<std::pair<std::string, ObjectHolder> > &arguments)
{
  std::vector<ObjectHolder> args(2*arguments.size() + 1);
  args[0] = ObjectHolder(command);
  for (size_t i = 0; i < arguments.size(); ++i)
  {
    const std::string &option = std::string("-") + arguments[i].first;
    args[2*i + 1] = ObjectHolder(option);
    ObjectHolder       value = arguments[i].second;
    args[2*i + 2] = value;
  }
  return RunCommand(args);
}

bool Interpreter::RunCommand(const std::string &commandString)
{
  Tcl_Interp *interp_ = reinterpret_cast<Tcl_Interp *>(GetMainInterpreter());
  int ret = Tcl_Eval(interp_, commandString.c_str());

  if (ret == TCL_OK)
  {
    error_string_.clear();
    result_ = ObjectHolder(Tcl_GetObjResult(interp_));
  }
  else
  {
    error_string_ = Tcl_GetStringResult(interp_);
    result_ = ObjectHolder();
  }

  return (ret == TCL_OK);
}

std::string Interpreter::GetVariable(const std::string &name)
{
  Tcl_Interp *interp_ = reinterpret_cast<Tcl_Interp *>(GetMainInterpreter());
  std::string result;
  const char * const val = Tcl_GetVar(interp_, name.c_str(), 0);
  if (val)
  {
    result = val;
  }
  return result;
}

