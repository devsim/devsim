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
#include "DefaultDerivatives.hh"
#include "FPECheck.hh"
#include "ObjectHolder.hh"

void devsim_initialization()
{
    FPECheck::InitializeFPE();

    GlobalData &gdata = GlobalData::GetInstance();

    dsHelper::CreateDefaultDerivatives();

    ObjectHolderMap_t features;
    features["version"] = ObjectHolder(DEVSIM_VERSION_STRING);
    features["copyright"] = ObjectHolder(u8"Copyright © " DEVSIM_COPYRIGHT_YEAR u8" DEVSIM LLC");
#ifdef DEVSIM_EXTENDED_PRECISION
    features["extended_precision"] = ObjectHolder(true);
#else
    features["extended_precision"] = ObjectHolder(false);
#endif
#ifdef USE_MKL_PARDISO
    features["direct_solver"] = ObjectHolder("mkl_pardiso");
#else
    features["direct_solver"] = ObjectHolder("superlu");
#endif
    features["license"] = ObjectHolder("Apache License, Version 2.0");
    features["website"] = ObjectHolder("https://devsim.org");
    gdata.AddDBEntryOnGlobal("info", ObjectHolder(features));
}

