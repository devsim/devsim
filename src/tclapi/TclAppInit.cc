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

#include "TclAppInit.hh"
#include "TclCommands.hh"
#include "OutputStream.hh"
#include "GetGlobalParameter.hh"
#include <tcl.h>

namespace mthread {
  Tcl_ThreadId mthread;
  extern const char *const mthread_error = "DEVSIM commands outside of main application thread not supported.";
}


int Tcl_AppInit(Tcl_Interp *interp) {
      SetMainInterpreter(interp);
      OutputStream::SetInterpreter(interp);
      OutputStream::WriteOut(OutputStream::INFO,
"\n"
"----------------------------------------\n"
"\n"
" DEVSIM\n"
"\n"
" Version: " DEVSIM_VERSION_STRING "\n"
"\n"
//// UTF-8 code.
" Copyright " DEVSIM_COPYRIGHT_YEAR " Devsim LLC\n"
"\n"
"----------------------------------------\n"
"\n"
"\n"
    );

      /* Initialize Tcl */
      if (Tcl_Init(interp) == TCL_ERROR) {
           return TCL_ERROR;
      }

      mthread::mthread = Tcl_GetCurrentThread();

      /* Initialize our extension */
      if (dsTcl::Commands_Init(interp) == TCL_ERROR) {
           return TCL_ERROR;
      }
      return TCL_OK;
}

