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
#include "ceval.h"
#include "frameobject.h"

#include "PythonCommands.hh"
#include "Interpreter.hh"
#include "CommandHandler.hh"
#include "GetArgs.hh"
#include "dsCommand.hh"
#include "Validate.hh"

#include "GeometryCommands.hh"
#include "MaterialCommands.hh"
#include "ModelCommands.hh"
#include "MathCommands.hh"
#include "EquationCommands.hh"
#include "MeshingCommands.hh"
#include "CircuitCommands.hh"
#include "dsException.hh"
#include "FPECheck.hh"
#include "OutputStream.hh"

#include <new>
#include <sstream>
#include <iostream>

using namespace dsValidate;

namespace dsPy {

namespace {
  std::map<std::string, newcmd> CommandMap;
  ObjectHolder devsim_exception;
}

PyObject *
CmdDispatch(PyObject *self, PyObject *args, PyObject *kwargs)
{
  //// return exception
  PyObject *ret = NULL;

  FPECheck::ClearFPE();

  std::string command_name;


  if (args)
  {
    if (PyTuple_CheckExact(args))
    {
      size_t argc = PyTuple_Size(args);
      if (argc == 1)
      {
        PyObject *lobj = PyTuple_GetItem(args, 0);
        if (PyString_CheckExact(lobj))
        {
          command_name = PyString_AsString(lobj);
        }
        else
        {
          PyErr_SetString(reinterpret_cast<PyObject *>(devsim_exception.GetObject()), "UNEXPECTED");
          return ret;
        }
      }
      else if (argc > 1)
      {
        std::ostringstream os;
        os << "Command " << command_name << " does not take positional arguments";
        PyErr_SetString(reinterpret_cast<PyObject *>(devsim_exception.GetObject()), os.str().c_str());
        return ret;
      }
    }
    else
    {
      PyErr_SetString(reinterpret_cast<PyObject *>(devsim_exception.GetObject()), "UNEXPECTED");
      return ret;
    }
  }

  std::map<std::string, newcmd>::iterator it = CommandMap.find(command_name);
  newcmd cmdptr = (it->second);
  if (it == CommandMap.end())
  {
    PyErr_SetString(reinterpret_cast<PyObject *>(devsim_exception.GetObject()), "UNEXPECTED");
    return ret;
  }

  try
  {

    dsGetArgs::CommandInfo command_info;
    command_info.self_         =  self;
    command_info.args_         =  args;
    command_info.kwargs_       =  kwargs;
    command_info.command_name_ =  command_name;
    command_info.exception_    =  reinterpret_cast<PyObject *>(devsim_exception.GetObject());

    CommandHandler data(&command_info);
    command_info.command_handler_ = &data;
    cmdptr(data);
    ret = reinterpret_cast<PyObject *>(data.GetReturnObject().GetObject());
    if (ret)
    {
      Py_INCREF(ret);
    }
  }
  catch (const dsException &x)
  {
    ret = NULL;
    PyErr_SetString(reinterpret_cast<PyObject *>(devsim_exception.GetObject()), x.what());
  }
  catch (std::bad_alloc &)
  {
    ret = NULL;
    PyErr_SetString(reinterpret_cast<PyObject *>(devsim_exception.GetObject()), const_cast<char *>("OUT OF MEMORY"));
  }
  catch (std::exception &)
  {
    ret = NULL;
    PyErr_SetString(reinterpret_cast<PyObject *>(devsim_exception.GetObject()), const_cast<char *>("UNEXPECTED ERROR"));
  }


  if (FPECheck::CheckFPE())
  {
    std::ostringstream os;
    os << "There was an uncaught floating point exception of type \"" << FPECheck::getFPEString() << "\"\n";
    FPECheck::ClearFPE();
    ret = NULL;
    OutputStream::WriteOut(OutputStream::ERROR, os.str().c_str());
  }


  return ret;
}

namespace {
bool CreateCommand(Interpreter &interp, const std::string &name)
{
  std::ostringstream os;
  os << "def " << name << "(*args, **kwargs):\n"
    "  return ds.devsim('" << name << "', *args, **kwargs)\n"
    "ds." << name << "=" << name << "\n"
    "del " << name << "\n"
    ;
  bool ok = interp.RunCommand(os.str());
  return ok;
}
}

void AddCommands(dsCommand::Commands *clistlist[])
{

  static struct PyMethodDef devsim_methods[] = {
    {"devsim", reinterpret_cast<PyCFunction>(CmdDispatch), METH_KEYWORDS, "devsim interpreter"},
    {NULL, NULL, 0, NULL}
  };
  PyObject *module = Py_InitModule("ds", devsim_methods);
  PyObject *ex = PyErr_NewException(const_cast<char *>("ds.error"), NULL, NULL);
  devsim_exception = ObjectHolder(ex);
  PyModule_AddObject(module, "error", ex);

  Interpreter interp;
  interp.RunCommand("import ds\n");
  dsCommand::Commands **tlist = clistlist;
  for ( ; (*tlist) != NULL; ++tlist)
  {
    dsCommand::Commands *clist = *tlist;
    for ( ; (clist->name) != NULL; ++clist)
    {
      CommandMap[clist->name] = clist->command;
      CreateCommand(interp, clist->name);
    }
  }
}

bool 
Commands_Init() {
    bool ok = false;

    ok = true;
    if (ok)
    {
      dsCommand::Commands *clistlist[] =
      {
        dsCommand::GeometryCommands,
        dsCommand::MaterialCommands,
        dsCommand::ModelCommands,
        dsCommand::MathCommands,
        dsCommand::EquationCommands,
        dsCommand::MeshingCommands,
        dsCommand::CircuitCommands,
        NULL
      };
      AddCommands(clistlist);
    }

    return ok;
}
}

