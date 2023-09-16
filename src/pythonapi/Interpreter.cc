/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "Python.h"
#include "Interpreter.hh"
#include "dsAssert.hh"
#include "GetGlobalParameter.hh"
#include "ControlGIL.hh"

Interpreter::Interpreter()
{
}

Interpreter::~Interpreter()
{
}

namespace
{
PyObject *GetGlobalDictionary()
{
  EnsurePythonGIL gil;

  PyObject *main    = PyImport_ImportModule("__main__");
  PyObject *globals = PyModule_GetDict(main);
  return globals;
}

PyObject *GetDevsimDictionary()
{
  EnsurePythonGIL gil;

  std::string module_name = TOSTRING(DEVSIM_MODULE_NAME);
  std::string long_module_name = "devsim." + module_name;

  PyObject *devsim = nullptr;
  devsim = PyImport_ImportModule(long_module_name.c_str());
  if (!devsim)
  {
    devsim = PyImport_ImportModule(module_name.c_str());
  }

  //PyErr_Print();
  dsAssert(devsim, std::string("Issue loading module ") + module_name);
  PyObject *globals = PyModule_GetDict(devsim);
  return globals;
}

ObjectHolder GetCommandFromInterpreter(const std::string &commandname, std::string &error_string)
{
  EnsurePythonGIL gil;

  ObjectHolder ret;

  PyObject *globals = nullptr;

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
  EnsurePythonGIL gil;

  ObjectHolder ret;

  PyObject *args = PyTuple_New(len);
  ret = ObjectHolder(args);

  for (size_t i = 0; i < len; ++i)
  {
    PyObject *p = reinterpret_cast<PyObject *>(objects[beg + i].GetObject());
    // PyTuple_SetItem steals a reference
    Py_INCREF(p);
    PyTuple_SetItem(args, i, p);
  }
  return ret;
}

#if 0
ObjectHolder CreateDictionary(const std::vector<std::pair<std::string, ObjectHolder> > &objects)
{
  ObjectHolder ret;
  PyObject *args = PyDict_New();
  ret = ObjectHolder(args);
  for (size_t i = 0; i < objects.size(); ++i)
  {
    const std::string &key = objects[i].first;
    const PyObject *obj = reinterpret_cast<const PyObject *>((objects[i].second).GetObject());
    //PyDict_SetItemString does not steal a reference
    //Py_INCREF(const_cast<PyObject *>(obj));
    PyDict_SetItemString(args, key.c_str(), const_cast<PyObject *>(obj));
  }
  return ret;
}
#endif

void ProcessError(const std::string &commandname, std::string &error_string)
{
  EnsurePythonGIL gil;

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

bool Interpreter::RunCommand(ObjectHolder &procedure, std::vector<ObjectHolder> &objects)
{
  EnsurePythonGIL gil;

  bool ret = false;
  error_string_.clear();

  ObjectHolder args = CreateTuple(objects, 0, objects.size());

  PyErr_Clear();
  PyObject *robj = PyObject_Call(reinterpret_cast<PyObject *>(procedure.GetObject()), reinterpret_cast<PyObject *>(args.GetObject()), nullptr);
  result_ = ObjectHolder(robj);
  if (robj)
  {
    ret = true;
  }
  else
  {
    ret = false;
    ProcessError("Python Command", error_string_);
  }

  return ret;
}

bool Interpreter::RunInternalCommand(const std::string &commandname, const std::vector<std::pair<std::string, ObjectHolder> > &arguments)
{
  EnsurePythonGIL gil;

  std::string newname;
  newname.reserve(commandname.size() + 3);
  newname += "ds.";
  newname += commandname;

  ObjectHolderMap_t omap(arguments.begin(), arguments.end());

  return RunCommand(newname, omap);
}

bool Interpreter::RunCommand(ObjectHolder &procedure, ObjectHolderMap_t &arguments)
{
  EnsurePythonGIL gil;

  bool ret = false;
  error_string_.clear();

  ObjectHolder kwargs(arguments);

  PyErr_Clear();

  PyObject *args = PyTuple_New(0);
  ObjectHolder args_holder(args);
  PyObject *robj = PyObject_Call(reinterpret_cast<PyObject *>(procedure.GetObject()), args, reinterpret_cast<PyObject *>(kwargs.GetObject()));
  result_ = ObjectHolder(robj);
  if (robj)
  {
    ret = true;
  }
  else
  {
    ret = false;
    ProcessError("Python Command", error_string_);
  }
  return ret;
}

bool Interpreter::RunCommand(const std::string &commandname, ObjectHolderMap_t &arguments)
{
  bool ret = false;
  error_string_.clear();


  ObjectHolder command_object = GetCommandFromInterpreter(commandname, error_string_);
  if (!error_string_.empty())
  {
    ret = false;
    return ret;
  }

  return RunCommand(command_object, arguments);

}

std::string Interpreter::GetVariable(const std::string &name)
{
  EnsurePythonGIL gil;

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

