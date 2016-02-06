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

#include "GlobalData.hh"
#include "TimeData.hh"
#include "NodeKeeper.hh"
#include "InstanceKeeper.hh"
#include "DefaultDerivatives.hh"
#include "MeshKeeper.hh"
#include "FPECheck.hh"
#include "MathEval.hh"
#include "MaterialDB.hh"
#include <tcl.h>
#include <cstdio>

int main(int argc, char * argv[])
{
// fix incorrect Microsoft Visual C++ formatting
//https://connect.microsoft.com/VisualStudio/feedback/details/1368280
#if 0
#if _WIN32
    _set_output_format(_TWO_DIGIT_EXPONENT);
#endif
#endif

    FPECheck::InitializeFPE();


    GlobalData &GInst = GlobalData::GetInstance();

    dsHelper::CreateDefaultDerivatives();

    Tcl_Main(argc, argv, Tcl_AppInit);

    //// These functions never actually get called since Tcl_Main calls exit!
    MaterialDB::DestroyInstance();
    GInst.DestroyInstance();
    dsMesh::MeshKeeper::DestroyInstance();
    NodeKeeper::delete_instance();
    InstanceKeeper::delete_instance();
    MathEval::DestroyInstance();
    TimeData::DestroyInstance();
}


