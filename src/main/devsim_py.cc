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
    features["copyright"] = ObjectHolder(u8"Copyright \u00a9 " DEVSIM_COPYRIGHT_YEAR " DEVSIM LLC");
#ifdef DEVSIM_EXTENDED_PRECISION
    features["extended_precision"] = ObjectHolder(true);
#else
    features["extended_precision"] = ObjectHolder(false);
#endif
    ObjectHolderList_t solvers;
#if defined (USE_MKL_PARDISO)
    solvers.push_back(ObjectHolder("mkl_pardiso"));
#endif
#if defined (USE_SUPERLU_PRECONDITIONER)
    solvers.push_back(ObjectHolder("superlu"));
#endif
    features["direct_solver"] = ObjectHolder(solvers);
    features["license"] = ObjectHolder("Apache License, Version 2.0");
    features["website"] = ObjectHolder("https://devsim.org");

#if defined(LOAD_MATHLIBS)
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
#elif defined(USE_SUPERLU_PRECONDITIONER)
    gdata.AddDBEntryOnGlobal("direct_solver", ObjectHolder("superlu"));
#endif

    gdata.AddDBEntryOnGlobal("info", ObjectHolder(features));
    gdata.AddDBEntryOnGlobal("symbolic_iteration_limit", ObjectHolder(1));
}

