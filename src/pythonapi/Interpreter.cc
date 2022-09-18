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

#include "Python.h"
#include "Interpreter.hh"
#include "dsAssert.hh"
#include "GetGlobalParameter.hh"


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
  PyObject *main    = PyImport_ImportModule("__main__");
  PyObject *globals = PyModule_GetDict(main);
  return globals;
}

PyObject *GetDevsimDictionary()
{
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

bool Interpreter::RunCommand(ObjectHolder &procedure, std::vector<ObjectHolder> &objects)
{
  bool ret = false;
  error_string_.clear();

  if (!error_string_.empty())
  {
    ret = false;
    return ret;
  }

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

