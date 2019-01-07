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
#include "OutputStream.hh"
#include "dsException.hh"
#include "GetGlobalParameter.hh"
#include "ObjectHolder.hh"
#include <iostream>

void *OutputStream::interpreter = nullptr;

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
    throw dsException();
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

