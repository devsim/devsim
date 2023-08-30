/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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
#include "DevsimImport.hh"

#include <new>
#include <sstream>

// Stringify
// borrowed from dsAssert.hh
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define PASTETOKENS(x, y) x ## y
// Module initialization function
#define HELPER1(x) PASTETOKENS(PyInit_, x)
#define DEVSIM_MODULE_INIT HELPER1(DEVSIM_MODULE_NAME)
#define DEVSIM_MODULE_STRING TOSTRING(DEVSIM_MODULE_NAME)

// Defined in devsim_py.cc
void devsim_initialization();

using namespace dsValidate;

namespace dsPy {
namespace {
struct module_state {
  module_state() : error(nullptr) {};
  PyObject *error;
};

#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))
}


static PyObject * CmdDispatch(PyObject *m, PyObject *args, PyObject *kwargs, const char *name, newcmd fptr)
{
  PyObject *ret = nullptr;

  FPECheck::ClearFPE();

  std::string command_name = name;


  if (args)
  {
    if (PyTuple_CheckExact(args))
    {
      size_t argc = PyTuple_Size(args);
      if (argc != 0)
      {
        std::ostringstream os;
        os << "Command " << command_name << " does not take positional arguments";
        PyErr_SetString(GETSTATE(m)->error, os.str().c_str());
        return ret;
      }
    }
    else
    {
      PyErr_SetString(GETSTATE(m)->error, "UNEXPECTED");
      return ret;
    }
  }

  std::string errorString;
  bool        isError = false;

  try
  {

    dsGetArgs::CommandInfo command_info;
    command_info.self_         =  m;
    command_info.args_         =  args;
    if (kwargs) {
    Py_INCREF(kwargs);
    }
    command_info.kwargs_       =  kwargs;
    command_info.command_name_ =  command_name;
    command_info.exception_    =  GETSTATE(m)->error;

    CommandHandler data(&command_info);
    command_info.command_handler_ = &data;
    fptr(data);

    isError = data.GetReturnCode() == 0;
    errorString = data.GetErrorString();

    ret = reinterpret_cast<PyObject *>(data.GetReturnObject().GetObject());

    if (ret)
    {
      if (!isError)
      {
        Py_INCREF(ret);
      }
      else
      {
        errorString += "UNEXPECTED: Error return with returned object\n";
        ret = nullptr;
      }
    }

  }
  catch (const dsException &x)
  {
    ret = nullptr;
    errorString = x.what();
  }
  catch (std::bad_alloc &x)
  {
    ret = nullptr;
    errorString = "OUT OF MEMORY\n";
    errorString += x.what();
  }
  catch (std::exception &x)
  {
    ret = nullptr;
    errorString = "UNEXPECTED ERROR\n";
    errorString += x.what();
  }

  if (FPECheck::CheckFPE())
  {
    std::ostringstream os;
    os << "Uncaught FPE: There was an uncaught floating point exception of type \"" << FPECheck::getFPEString() << "\"\n";
    errorString += os.str();

    FPECheck::ClearFPE();

    if (ret)
    {
      Py_DECREF(ret);
      ret = nullptr;
    }
  }

  if (!ret)
  {
    PyErr_SetString(GETSTATE(m)->error, const_cast<char *>(errorString.c_str()));
  }

  return ret;
}

// https://gcc.gnu.org/onlinedocs/cpp/Stringification.html
// https://gcc.gnu.org/onlinedocs/cpp/Concatenation.html
#define MyNewPyPtr(name, fptr) \
PyObject * name ## CmdDispatch(PyObject *self, PyObject *args, PyObject *kwargs) \
{ \
  return CmdDispatch(self, args, kwargs, # name, fptr); \
}

#define  DS_FUNCTION_TABLE MyNewPyPtr
#include "CommandTable.cc"
#undef   DS_FUNCTION_TABLE


#include "DevsimDoc.cc"

// https://stackoverflow.com/questions/28857522/python-c-extension-keyword-arguments
#define MYCOMMAND(name, fptr) {#name, reinterpret_cast<PyCFunction>(name ## CmdDispatch), METH_KEYWORDS | METH_VARARGS, name ## _doc},

#define DS_FUNCTION_TABLE MYCOMMAND
static struct PyMethodDef devsim_methods[] = {
//    {"devsim", reinterpret_cast<PyCFunction>(CmdDispatch), METH_KEYWORDS, "devsim interpreter"},
#include "CommandTable.cc"
{nullptr, nullptr, 0, nullptr}
};
#undef   DS_FUNCTION_TABLE

#ifndef _WIN32
static int devsim_traverse(PyObject *m, visitproc visit, void *arg) {
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int devsim_clear(PyObject *m) {
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}
#endif

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        DEVSIM_MODULE_STRING,
        nullptr,
        sizeof(struct module_state),
        devsim_methods,
        nullptr,
#ifdef _WIN32
// Fix memory error found on Windows 10
// but no other platforms
        nullptr,
        nullptr,
#else
        devsim_traverse,
        devsim_clear,
#endif
        nullptr
};

#define INITERROR return nullptr

//PyMODINIT_FUNC // this next line is used instead to use DLL_PUBLIC macro
extern "C" DLL_PUBLIC PyObject *
DEVSIM_MODULE_INIT(void)
{
  PyObject *module = PyModule_Create(&moduledef);

    if (module == nullptr)
    {
      INITERROR;
    }

    // TODO: modify symdiff to use this approach
    struct module_state *st = GETSTATE(module);
    st->error = PyErr_NewException(const_cast<char *>(DEVSIM_MODULE_STRING ".error"), nullptr, nullptr);
    if (st->error == nullptr) {
        Py_DECREF(module);
        INITERROR;
    }

    PyModule_AddObject(module, "error", st->error);

    //https://www.python.org/dev/peps/pep-0396/
    PyDict_SetItemString(PyModule_GetDict(module), "__version__", PyUnicode_FromString(DEVSIM_VERSION_STRING));

    try {
      devsim_initialization();
    }
    catch (...)
    {
        PyErr_SetString(PyExc_RuntimeError, "Issues initializing DEVSIM.");
        Py_DECREF(module);
        INITERROR;
    }

  return module;
}
}

