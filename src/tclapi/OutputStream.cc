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

#include "OutputStream.hh"
#include "dsException.hh"
#include "GetGlobalParameter.hh"
#include "ObjectHolder.hh"
#include <tcl.h>
#include <iostream>

void *OutputStream::interpreter = NULL;

void OutputStream::SetInterpreter(void *interp)
{
    interpreter = interp;
}

void OutputStream::WriteOut(OutputType ot, Verbosity_t verbosity, const std::string &msg)
{
  Tcl_Interp *tclInterp = reinterpret_cast<Tcl_Interp *>(interpreter);
  if (verbosity == UNKNOWN)
  {
    verbosity = V2;
  }

  if (!tclInterp)
  {
    Tcl_Exit(-1);
  }
  if (ot == INFO)
  {
    Tcl_Channel tc = Tcl_GetChannel(tclInterp, "stdout", NULL);
    if (!tc)
    {
      std::cerr << "Could not find output channel!";
      Tcl_Exit(-1);
    }
    Tcl_WriteChars(tc, msg.c_str(), -1);
    Tcl_Flush(tc);
  }
  else if (ot == VERBOSE1)
  {
    if ((verbosity == V1) || (verbosity == V2))
    {
      Tcl_Channel tc = Tcl_GetChannel(tclInterp, "stdout", NULL);
      if (!tc)
      {
          std::cerr << "Could not find output channel!";
          Tcl_Exit(-1);
      }
      Tcl_WriteChars(tc, msg.c_str(), -1);
      Tcl_Flush(tc);
    }
  }
  else if (ot == VERBOSE2)
  {
    if (verbosity == V2)
    {
      Tcl_Channel tc = Tcl_GetChannel(tclInterp, "stdout", NULL);
      if (!tc)
      {
          std::cerr << "Could not find output channel!";
          Tcl_Exit(-1);
      }
      Tcl_WriteChars(tc, msg.c_str(), -1);
      Tcl_Flush(tc);
    }
  }
  else if (ot == ERROR)
  {
    Tcl_Channel tc = Tcl_GetChannel(tclInterp, "stderr", NULL);
    if (!tc)
    {
      std::cerr << "Could not find output channel!";
      Tcl_Exit(-1);
    }
    Tcl_WriteChars(tc, msg.c_str(), -1);
    Tcl_Flush(tc);
  }
  else if (ot == FATAL)
  {
    Tcl_Channel tc = Tcl_GetChannel(tclInterp, "stderr", NULL);
    if (!tc)
    {
      std::cerr << "Could not find output channel!";
      Tcl_Exit(-1);
    }
    Tcl_WriteChars(tc, msg.c_str(), -1);
    Tcl_Flush(tc);
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

