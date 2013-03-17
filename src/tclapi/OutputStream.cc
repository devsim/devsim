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

