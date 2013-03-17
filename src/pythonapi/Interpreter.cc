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

#include "Python.h"
#include "Interpreter.hh"
#include "dsAssert.hh"
#include "GetGlobalParameter.hh"

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
//  PyObject *interp_ = reinterpret_cast<PyObject *>(GetMainInterpreter());

  std::vector<ObjectHolder> objs(arguments.size() + 1);
  objs[0] = ObjectHolder(command);
  for (size_t i = 0; i < arguments.size(); ++i)
  {
    objs[i + 1] = ObjectHolder(arguments[i]);
  }
  return RunCommand(objs);
}

namespace
{
//// this shouldn't fail
PyObject *GetGlobalDictionary()
{
  PyObject *main    = PyImport_ImportModule("__main__");
  PyObject *globals = PyModule_GetDict(main);
  return globals;
}

PyObject *GetDevsimDictionary()
{
  PyObject *devsim = PyImport_ImportModule("ds");
  PyObject *globals = PyModule_GetDict(devsim);
  return globals;
}

ObjectHolder GetCommandFromInterpreter(const std::string &commandname, std::string &error_string)
{
  ObjectHolder ret;

  PyObject *globals = NULL;

  std::string search_name;

  if (commandname.find("ds.") == 0)
  {
    globals     = GetDevsimDictionary();
    search_name = commandname.substr(3);
  }
  else
  {
    globals = GetGlobalDictionary();
    search_name = commandname;
  }

  ObjectHolder cno(search_name);

  if (!PyDict_Contains(globals, reinterpret_cast<PyObject *>(cno.GetObject())))
  {
    error_string =  commandname + " not found";
    return ret;
  }

  PyObject *fobj = PyDict_GetItem(globals, reinterpret_cast<PyObject *>(cno.GetObject()));

  if (PyCallable_Check(fobj))
  {
    Py_INCREF(fobj);
    ret = ObjectHolder(fobj);
  }
  else
  {
    error_string += commandname + " not callable";
  }
  return ret;
}


ObjectHolder CreateTuple(std::vector<ObjectHolder> &objects, size_t beg, size_t len)
{
  ObjectHolder ret;

  PyObject *args = PyTuple_New(len);
  ret = ObjectHolder(args);

  for (size_t i = 0; i < len; ++i)
  {
    PyObject *p = reinterpret_cast<PyObject *>(objects[beg + i].GetObject());
    Py_INCREF(p);
    PyTuple_SET_ITEM(args, i, p);
  } 
  return ret;
}

///// Check for duplicates!
ObjectHolder CreateDictionary(const std::vector<std::pair<std::string, ObjectHolder> > &objects)
{
  ObjectHolder ret;
  PyObject *args = PyDict_New();
  ret = ObjectHolder(args);
  for (size_t i = 0; i < objects.size(); ++i)
  {
    const std::string &key = objects[i].first;
    const PyObject *obj = reinterpret_cast<const PyObject *>((objects[i].second).GetObject());
    Py_INCREF(const_cast<PyObject *>(obj));
    PyDict_SetItemString(args, key.c_str(), const_cast<PyObject *>(obj));
  }
  return ret;
}

void ProcessError(const std::string &commandname, std::string &error_string)
{
  if (PyErr_Occurred())
  {
    PyObject *ptype;
    PyObject *pvalue;
    PyObject *ptraceback;
    PyErr_Fetch(&ptype, &pvalue, &ptraceback);
    ObjectHolder type(ptype);
    ObjectHolder value(pvalue);
    ObjectHolder traceback(ptraceback);
    error_string += commandname + " failed\n";
    error_string += type.GetString() + "\n" + value.GetString() + "\n";
  }
}
}


bool Interpreter::RunCommand(std::vector<ObjectHolder> &objects)
{
  bool ret = false;
  error_string_.clear();

  std::string commandname = objects[0].GetString();

  ObjectHolder command_object = GetCommandFromInterpreter(commandname, error_string_);
  if (!error_string_.empty())
  {
    ret = false;
    return ret;
  }

  //// This really can't fail
  ObjectHolder args = CreateTuple(objects, 1, objects.size() - 1);

  PyErr_Clear();
  PyObject *robj = PyObject_Call(reinterpret_cast<PyObject *>(command_object.GetObject()), reinterpret_cast<PyObject *>(args.GetObject()), NULL);
  result_ = ObjectHolder(robj);
  if (robj)
  {
    ret = true;
  }
  else
  {
    ret = false;
    ProcessError(commandname, error_string_);
  }

  return ret;
}

bool Interpreter::RunInternalCommand(const std::string &commandname, const std::vector<std::pair<std::string, ObjectHolder> > &arguments)
{
  std::string newname;
  newname.reserve(commandname.size() + 3);
  newname += "ds.";
  newname += commandname;
  return RunCommand(newname, arguments);
}

bool Interpreter::RunCommand(const std::string &commandname, const std::vector<std::pair<std::string, ObjectHolder> > &arguments)
{
  bool ret = false;
  error_string_.clear();


  ObjectHolder command_object = GetCommandFromInterpreter(commandname, error_string_);
  if (!error_string_.empty())
  {
    ret = false;
    return ret;
  }

  ObjectHolder kwargs = CreateDictionary(arguments);

  PyErr_Clear();

  PyObject *args = PyTuple_New(0);
  ObjectHolder args_holder(args);
  PyObject *robj = PyObject_Call(reinterpret_cast<PyObject *>(command_object.GetObject()), args, reinterpret_cast<PyObject *>(kwargs.GetObject()));
  result_ = ObjectHolder(robj);
  if (robj)
  {
    ret = true;
  }
  else
  {
    ret = false;
    ProcessError(commandname, error_string_);
  }
  return ret;
}


bool Interpreter::RunCommand(const std::string &commandString)
{
  error_string_.clear();
  PyErr_Clear();
  PyObject *globals = GetGlobalDictionary();
  PyObject *res = PyRun_String(commandString.c_str(), Py_file_input, globals, globals);
  result_ = ObjectHolder(res);

  if (!res)
  {
    ProcessError(commandString, error_string_);
  }

  return (res != NULL);
}

std::string Interpreter::GetVariable(const std::string &name)
{
  std::string ret;
  PyObject *main = PyImport_ImportModule("__main__");
  ObjectHolder main_holder(main);
  PyObject *robj = PyObject_GetAttrString(main, name.c_str());
  ObjectHolder ro(robj);
  if (robj)
  {
    ret = ro.GetString();
  }
  return ret;
}

