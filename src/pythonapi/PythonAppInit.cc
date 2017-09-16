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
      OutputStream::WriteOut(OutputStream::OutputType::INFO,
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

