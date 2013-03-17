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
#include "PythonAppInit.hh"
#include "PythonCommands.hh"
#include "OutputStream.hh"
#include "GetGlobalParameter.hh"

extern "C" {
//int Sqlite3_Init(Tcl_Interp *interp);
}

#if 0
namespace mthread {
  Tcl_ThreadId mthread;
  extern const char *const mthread_error = "DEVSIM commands outside of main application thread not supported.";
}
#endif


bool Python_AppInit() {
    Py_Initialize();
//    PyEval_InitThreads();
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

    Py_SetProgramName(const_cast<char *>("DEVSIM"));
//    PyEval_ReleaseLock();

#if 0
      if (Tcl_PkgRequire(interp, "devsim_license", "1.0", 0) == NULL)
      {
        return TCL_ERROR;
      }

      mthread::mthread = Tcl_GetCurrentThread();

      /* Initialize Sqlite3 */
      if (Sqlite3_Init(interp) == TCL_ERROR) {
           return TCL_ERROR;
      }
#endif

      /* Initialize our extension */
      if (dsPy::Commands_Init() == false) {
           return false;
      }
      return true;
}

