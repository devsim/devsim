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

#include "SolverUtil.hh"
#include "SuperLUPreconditioner.hh"
#include "BlockPreconditioner.hh"
#ifdef USE_MKL_PARDISO
#include "MKLPardisoPreconditioner.hh"
#endif
#include "ExternalPreconditioner.hh"
#include "LinearSolver.hh"
#include "IterativeLinearSolver.hh"
#include "CompressedMatrix.hh"
#include "BlasHeaders.hh"
#include "GlobalData.hh"
#include "ObjectHolder.hh"
#include "OutputStream.hh"
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
#endif

namespace {
enum class DirectSolver {
  UNKNOWN,
  MKLPARDISO,
  SUPERLU,
  CUSTOM,
};

DirectSolver GetDirectSolver()
{
  DirectSolver ret = DirectSolver::UNKNOWN;
  GlobalData &gdata = GlobalData::GetInstance();
  auto dbent = gdata.GetDBEntryOnGlobal("direct_solver");
  if (dbent.first)
  {
    const auto &val = dbent.second.GetString();
    if (val == "superlu")
    {
      ret = DirectSolver::SUPERLU;
    }
    else if (val == "mkl_pardiso")
    {
#ifdef USE_MKL_PARDISO
      ret = DirectSolver::MKLPARDISO;
#else
      ret = DirectSolver::SUPERLU;
      std::ostringstream os;
      os << "Solver \"mkl_pardiso\" not supported in this build.\n";
      OutputStream::WriteOut(OutputStream::OutputType::FATAL, os.str());
#endif
    }
    else if (val == "custom")
    {
      ret = DirectSolver::CUSTOM;
      if (auto csolver = gdata.GetDBEntryOnGlobal("solver_callback"); csolver.first)
      {
        if (!csolver.second.IsCallable())
        {
          ret = DirectSolver::UNKNOWN;
          std::ostringstream os;
          os << "Solver \"custom\" specified, but \"solver_callback\" not callable.\n";
          OutputStream::WriteOut(OutputStream::OutputType::FATAL, os.str());
        }
      }
      else
      {
        ret = DirectSolver::UNKNOWN;
        std::ostringstream os;
        os << "Solver \"custom\" specified, but \"solver_callback\" not set.\n";
        OutputStream::WriteOut(OutputStream::OutputType::FATAL, os.str());
      }
    }
    else
    {
      std::ostringstream os;
      os << "Unrecognized \"direct_solver\" parameter value \"" << val << "\". Valid options are \"mkl_pardiso\", \"superlu\", or \"custom\".\n";
      OutputStream::WriteOut(OutputStream::OutputType::FATAL, os.str());
    }
    return ret;
  }

  std::ostringstream os;
  os << "Parameter \"direct_solver\" parameter not set. Valid options are \"mkl_pardiso\" or \"superlu\".\n";
#if defined(USE_EXPLICIT_MATH_LOAD)
  if (MathLoader::IsMKLLoaded())
  {
    os << "Using \"mkl_pardiso\" direct solver.\n";
    ret = DirectSolver::MKLPARDISO;
  }
  else
  {
    os << "Using \"superlu\" direct solver.\n";
    ret = DirectSolver::SUPERLU;
  }
#elif defined(USE_MKL_PARDISO)
  os << "Using \"mkl_pardiso\" direct solver.\n";
  ret = DirectSolver::MKLPARDISO;
#else
  os << "Using \"superlu\" direct solver.\n";
  ret = DirectSolver::SUPERLU;
#endif
  OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
  return ret;
}

template <typename T>
dsMath::Preconditioner<T> *CreateExternalPreconditioner(size_t numeqns, dsMath::PEnum::TransposeType_t transtype, std::string &errorstring)
{
  // TODO: use helper function to encapsulate
  auto p = new dsMath::ExternalPreconditioner<T>(numeqns, transtype);
  auto &gdata = GlobalData::GetInstance();
  if (auto dbent = gdata.GetDBEntryOnGlobal("solver_callback"); dbent.first)
  {
    bool ret = p->init(dbent.second, errorstring);
    dsAssert(ret, errorstring);
  }
  else
  {
    dsAssert(dbent.first, "solver_callback not available");
  }
  return p;
}
}

namespace dsMath
{
template <>
Preconditioner<double> *CreatePreconditioner(LinearSolver<double> &itermethod, size_t numeqns)
{
  Preconditioner<double> *preconditioner = nullptr;
  if (dynamic_cast<IterativeLinearSolver<double> *>(&itermethod))
  {
    preconditioner = new BlockPreconditioner<double>(numeqns, PEnum::TransposeType_t::NOTRANS);
  }
  else
  {
    if (auto s = GetDirectSolver(); s == DirectSolver::SUPERLU)
    {
      preconditioner = new SuperLUPreconditioner<double>(numeqns, PEnum::TransposeType_t::NOTRANS, PEnum::LUType_t::FULL);
    }
#ifdef USE_MKL_PARDISO
    else if (s == DirectSolver::MKLPARDISO)
    {
      preconditioner = new MKLPardisoPreconditioner<double>(numeqns, PEnum::TransposeType_t::NOTRANS);
    }
#endif
    else if (s == DirectSolver::CUSTOM)
    {
      std::string errorstring;
      preconditioner = CreateExternalPreconditioner<double>(numeqns, PEnum::TransposeType_t::NOTRANS, errorstring);
      dsAssert(preconditioner, errorstring);
    }
    else
    {
      dsAssert(false, "Unexpected Solver Type");
    }
  }
  return preconditioner;
}

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
Preconditioner<float128> *CreatePreconditioner(LinearSolver<float128> &itermethod, size_t numeqns)
{
  Preconditioner<float128> *preconditioner = nullptr;
  if (dynamic_cast<IterativeLinearSolver<float128> *>(&itermethod))
  {
    preconditioner = new BlockPreconditioner<float128>(numeqns, PEnum::TransposeType_t::NOTRANS);
  }
  else
  {
    if (auto s = GetDirectSolver(); s == DirectSolver::SUPERLU)
    {
      preconditioner = new SuperLUPreconditioner<float128>(numeqns, PEnum::TransposeType_t::NOTRANS, PEnum::LUType_t::FULL);
    }
#ifdef USE_MKL_PARDISO
    else if (s == DirectSolver::MKLPARDISO)
    {
      preconditioner = new MKLPardisoPreconditioner<float128>(numeqns, PEnum::TransposeType_t::NOTRANS);
    }
#endif
    else if (s == DirectSolver::CUSTOM)
    {
      std::string errorstring;
      preconditioner = CreateExternalPreconditioner<float128>(numeqns, PEnum::TransposeType_t::NOTRANS, errorstring);
      dsAssert(preconditioner, errorstring);
    }
    else
    {
      dsAssert(false, "Unexpected Solver Type");
    }
  }
  return preconditioner;
}
#endif

template <typename DoubleType>
Preconditioner<DoubleType> *CreateACPreconditioner(PEnum::TransposeType_t trans_type, size_t numeqns)
{
    Preconditioner<DoubleType> *preconditioner = nullptr;
    if (auto s = GetDirectSolver();  s == DirectSolver::SUPERLU)
    {
      preconditioner = new SuperLUPreconditioner<DoubleType>(numeqns, trans_type, PEnum::LUType_t::FULL);
    }
#ifdef USE_MKL_PARDISO
    else if (s == DirectSolver::MKLPARDISO)
    {
      preconditioner = new MKLPardisoPreconditioner<DoubleType>(numeqns, trans_type);
    }
#endif
    else if (s == DirectSolver::CUSTOM)
    {
      std::string errorstring;
      preconditioner = CreateExternalPreconditioner<DoubleType>(numeqns, trans_type, errorstring);
      dsAssert(preconditioner, errorstring);
    }
    else
    {
      dsAssert(false, "Unexpected Solver Type");
    }
    return preconditioner;
}

template <typename DoubleType>
CompressedMatrix<DoubleType> *CreateMatrix(Preconditioner<DoubleType> *preconditioner, bool is_complex)
{
  const auto numeqns = preconditioner->size();

  auto matrix_type = MatrixType::REAL;
  auto compression_type = preconditioner->GetRealMatrixCompressionType();
  if (is_complex)
  {
    matrix_type = MatrixType::COMPLEX;
    compression_type = preconditioner->GetComplexMatrixCompressionType();
  }

  return new CompressedMatrix<DoubleType>(numeqns, matrix_type, compression_type);
}

template <typename DoubleType>
CompressedMatrix<DoubleType> *CreateACMatrix(Preconditioner<DoubleType> *preconditioner)
{
  return CreateMatrix(preconditioner, true);
}

template Preconditioner<double> *CreateACPreconditioner(PEnum::TransposeType_t trans_type, size_t numeqns);
template CompressedMatrix<double> *CreateMatrix(Preconditioner<double> *preconditioner, bool is_complex);
template CompressedMatrix<double> *CreateACMatrix(Preconditioner<double> *preconditioner);
#ifdef DEVSIM_EXTENDED_PRECISION
template Preconditioner<float128> *CreateACPreconditioner(PEnum::TransposeType_t trans_type, size_t numeqns);
template CompressedMatrix<float128> *CreateMatrix(Preconditioner<float128> *preconditioner, bool is_complex);
template CompressedMatrix<float128> *CreateACMatrix(Preconditioner<float128> *preconditioner);
#endif
}

