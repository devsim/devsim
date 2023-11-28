/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "Python.h"
#include "OutputStream.hh"
#include "dsException.hh"
#include "GetGlobalParameter.hh"
#include "ObjectHolder.hh"
#include "ControlGIL.hh"

#include <iostream>

void OutputStream::WriteOut(OutputType ot, Verbosity_t verbosity, const std::string &msg)
{
  EnsurePythonGIL gil;

  //// just assume the program has terminated
  if (!Py_IsInitialized())
  {
//    Py_Exit(-1);
    return;
  }

  if (verbosity == Verbosity_t::UNKNOWN)
  {
    verbosity = Verbosity_t::V2;
  }

  if (ot == OutputType::INFO)
  {
    PyObject *tc = PySys_GetObject(const_cast<char *>("stdout"));
    if (!tc)
    {
      std::cerr << "Could not find output channel!";
      Py_Exit(-1);
    }
    PyFile_WriteString(const_cast<char *>(msg.c_str()), tc);
    PyObject_CallMethod(tc, const_cast<char *>("flush"), const_cast<char *>(""));
  }
  else if (ot == OutputType::VERBOSE1)
  {
    if ((verbosity == Verbosity_t::V1) || (verbosity == Verbosity_t::V2))
    {
      PyObject *tc = PySys_GetObject(const_cast<char *>("stdout"));
      if (!tc)
      {
          std::cerr << "Could not find output channel!";
          Py_Exit(-1);
      }
      PyFile_WriteString(const_cast<char *>(msg.c_str()), tc);
      PyObject_CallMethod(tc, const_cast<char *>("flush"), const_cast<char *>(""));
    }
  }
  else if (ot == OutputType::VERBOSE2)
  {
    if (verbosity == Verbosity_t::V2)
    {
      PyObject *tc = PySys_GetObject(const_cast<char *>("stdout"));
      if (!tc)
      {
          std::cerr << "Could not find output channel!";
          Py_Exit(-1);
      }
      PyFile_WriteString(const_cast<char *>(msg.c_str()), tc);
      PyObject_CallMethod(tc, const_cast<char *>("flush"), const_cast<char *>(""));
    }
  }
  else if (ot == OutputType::ERROR)
  {
    PyObject *tc = PySys_GetObject(const_cast<char *>("stdout"));
    if (!tc)
    {
      std::cerr << "Could not find output channel!";
      Py_Exit(-1);
    }
    PyFile_WriteString(const_cast<char *>(msg.c_str()), tc);
    PyObject_CallMethod(tc, const_cast<char *>("flush"), const_cast<char *>(""));
  }
  else if (ot == OutputType::FATAL)
  {
    PyObject *tc = PySys_GetObject(const_cast<char *>("stdout"));
    if (!tc)
    {
      std::cerr << "Could not find output channel!";
      Py_Exit(-1);
    }
    PyFile_WriteString(const_cast<char *>(msg.c_str()), tc);
    PyObject_CallMethod(tc, const_cast<char *>("flush"), const_cast<char *>(""));
    throw dsException(msg);
  }
}


OutputStream::Verbosity_t OutputStream::GetVerbosity(const std::string &vstring)
{
  OutputStream::Verbosity_t ret = Verbosity_t::V0;
  if (!vstring.empty())
  {
    if (vstring == "info")
    {
      ret = Verbosity_t::V0;
    }
    else if (vstring == "verbose")
    {
      ret = Verbosity_t::V2;
    }
    else
    {
      ret = Verbosity_t::UNKNOWN;
    }
  }
  return ret;
}

void OutputStream::WriteOut(OutputType ot, const std::string &msg)
{
  std::string debug_level = GetGlobalParameterStringOptional("debug_level");
  OutputStream::WriteOut(ot, GetVerbosity(debug_level), msg);
}

