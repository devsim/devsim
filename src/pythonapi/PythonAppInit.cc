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
}


#if 0
bool Python_AppInit() {

#if PY_MAJOR_VERSION >=3
  /* Initialize our extension */
  if (dsPy::Commands_Init() == false) {
       return false;
  }
  Py_Initialize();
#else
  /* Initialize our extension */
  Py_Initialize();
  
  if (dsPy::Commands_Init() == false) {
       return false;
  }

#endif

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
  return true;
}
#endif

