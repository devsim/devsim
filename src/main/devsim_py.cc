/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "GlobalData.hh"
#include "DefaultDerivatives.hh"
#include "FPECheck.hh"
#include "ObjectHolder.hh"
#if defined(LOAD_MATHLIBS)
#include "BlasHeaders.hh"
#endif
#include "OutputStream.hh"
#include <sstream>

void devsim_initialization()
{
    FPECheck::InitializeFPE();

    GlobalData &gdata = GlobalData::GetInstance();

    dsHelper::CreateDefaultDerivatives();

    ObjectHolderMap_t features;
    features["version"] = ObjectHolder(DEVSIM_VERSION_STRING);
    features["copyright"] = ObjectHolder(reinterpret_cast<const char *>(u8"Copyright \u00a9 " DEVSIM_COPYRIGHT_YEAR " DEVSIM LLC"));
#ifdef DEVSIM_EXTENDED_PRECISION
    features["extended_precision"] = ObjectHolder(true);
#else
    features["extended_precision"] = ObjectHolder(false);
#endif

    std::string direct_solver;
#if defined (USE_MKL_PARDISO)
    direct_solver = "mkl_pardiso";
#endif
#if defined (USE_SUPERLU_PRECONDITIONER)
    direct_solver = "superlu";
#endif
    features["license"] = ObjectHolder("Apache License, Version 2.0");
    features["website"] = ObjectHolder("https://devsim.org");

#if defined(LOAD_MATHLIBS)
    std::string errors;
    MathLoader::LoaderMessages_t ret = MathLoader::LoaderMessages_t::NO_ENVIRONMENT;
    if (MathLoader::IsMathLoaded())
    {
      ret = MathLoader::GetMathStatus();
    }
    else
    {
      ret = MathLoader::LoadMathLibraries(errors);
    }
    if (ret == MathLoader::LoaderMessages_t::MKL_LOADED)
    {
      features["mkl_version"] = ObjectHolder(MathLoader::GetMKLVersion());
      direct_solver = "mkl_pardiso";
    }
    else if (ret == MathLoader::LoaderMessages_t::MATH_LOADED)
    {
      direct_solver = "unknown";
    }
    else
    {
      std::ostringstream os;
      os << "Error loading math libraries.  Please install a suitable BLAS/LAPACK library and set DEVSIM_MATH_LIBS.  Alternatively, install the Intel MKL.\n";
      OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
      OutputStream::WriteOut(OutputStream::OutputType::FATAL, errors);
    }
#elif defined(USE_SUPERLU_PRECONDITIONER)
    direct_solver = "superlu";
#endif

    features["direct_solver"] = ObjectHolder(direct_solver);
    features["math_libraries"] = CreateObjectHolderList(MathLoader::GetLoadedMathDLLs());
    features["blas_symbol_prefix"] = ObjectHolder(MathLoader::GetBLASPrefix());
    features["blas_symbol_suffix"] = ObjectHolder(MathLoader::GetBLASSuffix());
    gdata.AddDBEntryOnGlobal("direct_solver", ObjectHolder(direct_solver));
    gdata.AddDBEntryOnGlobal("info", ObjectHolder(features));
    gdata.AddDBEntryOnGlobal("symbolic_iteration_limit", ObjectHolder(1));
}

