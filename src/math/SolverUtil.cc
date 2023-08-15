/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "SolverUtil.hh"
#include "BlockPreconditioner.hh"
#ifdef USE_MKL_PARDISO
#include "MKLPardisoPreconditioner.hh"
#endif
#ifdef USE_SUPERLU_PRECONDITIONER
#include "SuperLUPreconditioner.hh"
#endif
#include "ExternalPreconditioner.hh"
#include "LinearSolver.hh"
#if defined(USE_ITERATIVE_SOLVER)
#include "IterativeLinearSolver.hh"
#endif
#include "CompressedMatrix.hh"
#if defined(LOAD_MATHLIBS)
#include "BlasHeaders.hh"
#endif
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
    if (val == "mkl_pardiso")
    {
#ifdef USE_MKL_PARDISO
      ret = DirectSolver::MKLPARDISO;
#else
      ret = DirectSolver::CUSTOM;
      std::ostringstream os;
      os << "Solver \"mkl_pardiso\" not supported in this build.\n";
      OutputStream::WriteOut(OutputStream::OutputType::FATAL, os.str());
#endif
    }
    else if (val == "superlu")
    {
#ifdef USE_SUPERLU_PRECONDITIONER
      ret = DirectSolver::SUPERLU;
#else
      ret = DirectSolver::CUSTOM;
      std::ostringstream os;
      os << "Solver \"superlu\" not supported in this build.\n";
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
      os << "Unrecognized \"direct_solver\" parameter value \"" << val << "\". Valid options are \"mkl_pardiso\", \"superlu\" or \"custom\".\n";
      OutputStream::WriteOut(OutputStream::OutputType::FATAL, os.str());
    }
    return ret;
  }

  std::ostringstream os;
  os << "Parameter \"direct_solver\" parameter not set. Valid options are \"mkl_pardiso\", or \"custom\".\n";

#if defined(USE_MKL_PARDISO)
  if (MathLoader::IsMKLLoaded())
  {
    os << "Using \"mkl_pardiso\" direct solver.\n";
    ret = DirectSolver::MKLPARDISO;
  }
  else
#endif
  {
    os << "Using \"custom\" direct solver.\n";
    ret = DirectSolver::CUSTOM;
  }

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
template <typename T>
Preconditioner<T> *CreateDirectPreconditioner(size_t numeqns)
{
  Preconditioner<T> *preconditioner = nullptr;

  if (auto s = GetDirectSolver(); s == DirectSolver::CUSTOM)
  {
    std::string errorstring;
    preconditioner = CreateExternalPreconditioner<T>(numeqns, PEnum::TransposeType_t::NOTRANS, errorstring);
    dsAssert(preconditioner, errorstring);
  }
#ifdef USE_MKL_PARDISO
  else if (s == DirectSolver::MKLPARDISO)
  {
    preconditioner = new MKLPardisoPreconditioner<T>(numeqns, PEnum::TransposeType_t::NOTRANS);
  }
#endif
#if defined (USE_SUPERLU_PRECONDITIONER)
  else if (s == DirectSolver::SUPERLU)
  {
    preconditioner = new SuperLUPreconditioner<T>(numeqns, PEnum::TransposeType_t::NOTRANS, PEnum::LUType_t::FULL);
  }
#endif
  else
  {
    dsAssert(false, "Unexpected Solver Type");
  }

  return preconditioner;
}

template <typename T>
Preconditioner<T> *CreatePreconditioner(LinearSolver<T> &itermethod, size_t numeqns)
{
  Preconditioner<T> *preconditioner = nullptr;

#if defined(LOAD_MATHLIBS)
  if (dynamic_cast<IterativeLinearSolver<T> *>(&itermethod))
  {
    preconditioner = new BlockPreconditioner<T>(numeqns, PEnum::TransposeType_t::NOTRANS);
  }
  else
#endif
  {
    preconditioner = CreateDirectPreconditioner<T>(numeqns);
  }

  return preconditioner;
}

template <typename DoubleType>
Preconditioner<DoubleType> *CreateACPreconditioner(PEnum::TransposeType_t trans_type, size_t numeqns)
{
    Preconditioner<DoubleType> *preconditioner = nullptr;

    if (auto s = GetDirectSolver(); s == DirectSolver::CUSTOM)
    {
      std::string errorstring;
      preconditioner = CreateExternalPreconditioner<DoubleType>(numeqns, trans_type, errorstring);
      dsAssert(preconditioner, errorstring);
    }
#ifdef USE_MKL_PARDISO
    else if (s == DirectSolver::MKLPARDISO)
    {
      preconditioner = new MKLPardisoPreconditioner<DoubleType>(numeqns, trans_type);
    }
#endif
#ifdef USE_SUPERLU_PRECONDITIONER
    else if (s == DirectSolver::SUPERLU)
    {
      preconditioner = new SuperLUPreconditioner<DoubleType>(numeqns, trans_type, PEnum::LUType_t::FULL);
    }
#endif
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

template Preconditioner<double> *CreateDirectPreconditioner(size_t numeqns);
template Preconditioner<double> *CreatePreconditioner(LinearSolver<double> &itermethod, size_t numeqns);
template Preconditioner<double> *CreateACPreconditioner(PEnum::TransposeType_t trans_type, size_t numeqns);
template CompressedMatrix<double> *CreateMatrix(Preconditioner<double> *preconditioner, bool is_complex);
template CompressedMatrix<double> *CreateACMatrix(Preconditioner<double> *preconditioner);
#ifdef DEVSIM_EXTENDED_PRECISION
template Preconditioner<float128> *CreateDirectPreconditioner(size_t numeqns);
template Preconditioner<float128> *CreatePreconditioner(LinearSolver<float128> &itermethod, size_t numeqns);
template Preconditioner<float128> *CreateACPreconditioner(PEnum::TransposeType_t trans_type, size_t numeqns);
template CompressedMatrix<float128> *CreateMatrix(Preconditioner<float128> *preconditioner, bool is_complex);
template CompressedMatrix<float128> *CreateACMatrix(Preconditioner<float128> *preconditioner);
#endif
}

