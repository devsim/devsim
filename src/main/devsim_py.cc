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

//#include "Python.h"
#include "GlobalData.hh"
#include "TimeData.hh"
#include "NodeKeeper.hh"
#include "InstanceKeeper.hh"
#include "DefaultDerivatives.hh"
#include "MeshKeeper.hh"
#include "FPECheck.hh"
#include "MathEval.hh"
#include "MaterialDB.hh"
#include "PythonAppInit.hh"
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
#endif
#include "OutputStream.hh"
#include <cstdio>



void devsim_initialization()
{
// fix incorrect Microsoft Visual C++ formatting
// https://connect.microsoft.com/VisualStudio/feedback/details/1368280
#if 0
#if _WIN32
    _set_output_format(_TWO_DIGIT_EXPONENT);
#endif
#endif

    FPECheck::InitializeFPE();

    GlobalData::GetInstance();

    dsHelper::CreateDefaultDerivatives();

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
}

#if 0
void devsim_finalization()
{
//// While this is correct for memory recovery, it creates issues in writing to the output stream after python has exited.
//// it appears that python may not support module unloading
#ifndef NDEBUG
    MaterialDB::DestroyInstance();
    GlobalData::GetInstance().DestroyInstance();
    dsMesh::MeshKeeper::DestroyInstance();
    NodeKeeper::delete_instance();
    InstanceKeeper::delete_instance();

    MathEval<double>::DestroyInstance();
    TimeData<double>::DestroyInstance();
#ifdef DEVSIM_EXTENDED_PRECISION
    MathEval<float128>::DestroyInstance();
    TimeData<float128>::DestroyInstance();
#endif
#endif
}

int devsim_main(int argc, char * argv[])
{

#if PY_MAJOR_VERSION >= 3
    std::vector<wchar_t *> wargv(argc);
    for (size_t i = 0; i < argc; ++i)
    {
      wargv[i] = Py_DecodeLocale(argv[i], nullptr);
    }
    Py_SetProgramName(wargv[0]);
#else
    Py_SetProgramName(argv[0]);
#endif

    Python_AppInit();

#if PY_MAJOR_VERSION >= 3
    int ret=Py_Main(argc, &wargv[0]);
    for (size_t i = 0; i < argc; ++i)
    {
      PyMem_RawFree(wargv[i]);
    }
    wargv.clear();
#else
    int ret=Py_Main(argc, argv);
#endif

    return ret;
}


int main(int argc, char * argv[])
{
  devsim_initialization();
  int ret = devsim_main(argc, argv);
  devsim_finalization();
}
#endif


