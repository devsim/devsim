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
#include "OutputStream.hh"
#include "dsException.hh"
#include "GetGlobalParameter.hh"
#include "ObjectHolder.hh"
#include <iostream>

void *OutputStream::interpreter = NULL;

void OutputStream::SetInterpreter(void *interp)
{
    interpreter = interp;
}

void OutputStream::WriteOut(OutputType ot, Verbosity_t verbosity, const std::string &msg)
{
  
// TODO: "do we need a thread lock"
  //// just assume the program has terminated
  if (!Py_IsInitialized())
  {
//    Py_Exit(-1);
    return;
  }

  if (verbosity == UNKNOWN)
  {
    verbosity = V2;
  }

  if (ot == INFO)
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
  else if (ot == VERBOSE1)
  {
    if ((verbosity == V1) || (verbosity == V2))
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
  else if (ot == VERBOSE2)
  {
    if (verbosity == V2)
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
  else if (ot == ERROR)
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
  else if (ot == FATAL)
  {
    PyObject *tc = PySys_GetObject(const_cast<char *>("stdout"));
    if (!tc)
    {
      std::cerr << "Could not find output channel!";
      Py_Exit(-1);
    }
    PyFile_WriteString(const_cast<char *>(msg.c_str()), tc);
    PyObject_CallMethod(tc, const_cast<char *>("flush"), const_cast<char *>(""));
    throw dsException();
  }
}


OutputStream::Verbosity_t OutputStream::GetVerbosity(const std::string &vstring)
{
  OutputStream::Verbosity_t ret = V0;
  if (!vstring.empty())
  {
    if (vstring == "info")
    {
      ret = V0;
    }
    else if (vstring == "verbose")
    {
      ret = V2; 
    }
    else
    {
      ret = UNKNOWN;
    }
  }
  return ret;
}

void OutputStream::WriteOut(OutputType ot, const std::string &msg)
{
  std::string debug_level = GetGlobalParameterStringOptional("debug_level");
  OutputStream::WriteOut(ot, GetVerbosity(debug_level), msg);
}

