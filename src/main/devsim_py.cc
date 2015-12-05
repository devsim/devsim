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
#include <cstdio>


int main(int argc, char * argv[])
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

    Python_AppInit();
    int ret=Py_Main(argc, argv);

//// While this is correct for memory recovery, it creates issues in writing to the output stream after python has exited.
#ifndef NDEBUG
    MaterialDB::DestroyInstance();
    GlobalData::GetInstance().DestroyInstance();
    dsMesh::MeshKeeper::DestroyInstance();
    NodeKeeper::delete_instance();
    InstanceKeeper::delete_instance();
    MathEval::DestroyInstance();
    TimeData::DestroyInstance();
#endif
    return ret;
}


