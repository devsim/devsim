/***
DEVSIM
Copyright 2023 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/
#include "ResetAllData.hh"
#include "EngineAPI.hh"
#include "InstanceKeeper.hh"
#include "NodeKeeper.hh"
#include "MeshKeeper.hh"
#include "GlobalData.hh"
#include "MathEval.hh"
#include "TimeData.hh"
#if defined(DEVSIM_EXTENDED_PRECISION)
#include "Float128.hh"
#endif
#if defined(USE_MATERIALDB)
#include "MaterialDB.hh"
#endif

void ResetAllData()
{
    InstanceKeeper::delete_instance();
    NodeKeeper::delete_instance();
    EngineAPI::ResetAllData();
    dsMesh::MeshKeeper::DestroyInstance();
    MathEval<double>::DestroyInstance();
#if defined(DEVSIM_EXTENDED_PRECISION)
    MathEval<float128>::DestroyInstance();
#endif
    TimeData<double>::DestroyInstance();
#if defined(DEVSIM_EXTENDED_PRECISION)
    TimeData<float128>::DestroyInstance();
#endif
#if defined(USE_MATERIALDB)
    MaterialDB::DestroyInstance();
#endif
    GlobalData::DestroyInstance();
}

