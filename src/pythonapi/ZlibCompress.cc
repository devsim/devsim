/***
DEVSIM
Copyright 2024 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/
#include "ZlibCompress.hh"
#include "ObjectHolder.hh"

#include "Python.h"

#include "dsAssert.hh"
#include "ControlGIL.hh"

#include <cstring>


bool DEVSIMZlibCompress(std::vector<char> &output, char *input, size_t input_length)
{
  EnsurePythonGIL gil;

  auto mod_importer = [](auto modname)->auto {
    ObjectHolder mod(PyImport_ImportModule(modname));
    PyErr_Clear();
    dsAssert(!mod.empty(), modname + " module not available");
    return mod;
  };

  auto get_callable = [](auto mod, auto fname)->auto
  {
    ObjectHolder fobj(PyObject_GetAttrString(reinterpret_cast<PyObject *>(mod.GetObject()), fname));
    PyErr_Clear();
    dsAssert(!fobj.empty(), fname + " not available");
    dsAssert(fobj.IsCallable(), fname + " is not callable");
    return fobj;
  };

  auto call_method = [](auto fobj, auto arg, auto error)->auto
  {
    ObjectHolder ret(PyObject_CallFunction(reinterpret_cast<PyObject *>(fobj.GetObject()), "O", arg.GetObject()));
    PyErr_Clear();
    dsAssert(!ret.empty(), error);
    return ret;
  };

  auto compress_method = get_callable(mod_importer("zlib"), "compress");

#if 1
// https://github.com/python/cpython/issues/98680
#ifndef PyBUF_READ
#define PyBUF_READ 0x100
#endif
  auto double_data = ObjectHolder(PyMemoryView_FromMemory(input, input_length, PyBUF_READ));
#else
  // TODO: "consider using memory view for Vector array creation form c++"
  auto double_data = ObjectHolder(input, input_length);
#endif

  auto compressed_data = call_method(compress_method, double_data, "issue compressing data");

  auto pyobj = reinterpret_cast<PyObject *>(compressed_data.GetObject());

  auto s = PyBytes_AsString(pyobj);
  PyErr_Clear();
  dsAssert(s, "Error converting to compressed base 64 string");

  auto l = PyBytes_Size(pyobj);

  output.resize(l);
  std::memcpy(output.data(), s, l);

  return true;
}
