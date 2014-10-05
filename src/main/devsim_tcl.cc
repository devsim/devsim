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
#if WIN32
    _set_output_format(_TWO_DIGIT_EXPONENT);
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


