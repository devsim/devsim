/***
DEVSIM
Copyright 2013 DEVSIM LLC

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
#include "BlasHeaders.hh"
#include "OutputStream.hh"
#include <sstream>

void devsim_initialization()
{
    FPECheck::InitializeFPE();

    GlobalData &gdata = GlobalData::GetInstance();

    dsHelper::CreateDefaultDerivatives();

    ObjectHolderMap_t features;
    features["version"] = ObjectHolder(DEVSIM_VERSION_STRING);
    features["copyright"] = ObjectHolder(u8"Copyright \u00a9 " DEVSIM_COPYRIGHT_YEAR " DEVSIM LLC");
#ifdef DEVSIM_EXTENDED_PRECISION
    features["extended_precision"] = ObjectHolder(true);
#else
    features["extended_precision"] = ObjectHolder(false);
#endif
    ObjectHolderList_t solvers;
#ifdef USE_MKL_PARDISO
    solvers.push_back(ObjectHolder("mkl_pardiso"));
#else
    solvers.push_back(ObjectHolder("unknown"));
    features["direct_solver"] = ObjectHolder(solvers);
#endif
    features["license"] = ObjectHolder("Apache License, Version 2.0");
    features["website"] = ObjectHolder("https://devsim.org");

    std::string errors;
    auto ret = MathLoader::LoadMathLibraries(errors);
    if (ret == MathLoader::LoaderMessages_t::MKL_LOADED)
    {
      features["mkl_version"] = ObjectHolder(MathLoader::GetMKLVersion());
      gdata.AddDBEntryOnGlobal("direct_solver", ObjectHolder("mkl_pardiso"));
    }
    else if (ret == MathLoader::LoaderMessages_t::MATH_LOADED)
    {
      gdata.AddDBEntryOnGlobal("direct_solver", ObjectHolder("unknown"));
    }
    else
    {
      std::ostringstream os;
      os << "Error loading math libraries.  Please install a suitable BLAS/LAPACK library and set DEVSIM_MATH_LIBS.  Alternatively, install the Intel MKL.\n";
      OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
      OutputStream::WriteOut(OutputStream::OutputType::FATAL, errors);
    }
    features["math_libraries"] = CreateObjectHolderList(MathLoader::GetLoadedMathDLLs());

    gdata.AddDBEntryOnGlobal("info", ObjectHolder(features));
}

