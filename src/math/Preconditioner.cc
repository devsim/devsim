/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "Preconditioner.hh"
#include "dsAssert.hh"
#include "Matrix.hh"
#include "FPECheck.hh"
#include "OutputStream.hh"
namespace dsMath {
template <typename DoubleType>
Preconditioner<DoubleType>::~Preconditioner()
{
}

template <typename DoubleType>
Preconditioner<DoubleType>::Preconditioner(size_t numeqns, PEnum::TransposeType_t transpose) : size_(numeqns), factored(false), transpose_solve_(transpose), matrix_(nullptr)
{
}

template <typename DoubleType>
bool Preconditioner<DoubleType>::GetTransposeSolve()
{
  return (transpose_solve_ == PEnum::TransposeType_t::TRANS);
}

template <typename DoubleType>
bool Preconditioner<DoubleType>::LUFactor(Matrix<DoubleType> *mat)
{

  factored = false;
  matrix_ = mat;

  FPECheck::ClearFPE();

  bool ret = this->DerivedLUFactor(matrix_);

#if (defined(__arm64__) && defined(__APPLE__)) || defined(__aarch64__)
  FPECheck::ClearFPE();
#endif

  if (FPECheck::CheckFPE())
  {
    std::ostringstream os;
    os << "There was a floating point exception of type \"" << FPECheck::getFPEString() << "\"  during LU Factorization\n";
    OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str().c_str());
    FPECheck::ClearFPE();
    ret = false;
  }

  factored = ret;
  return ret;
}


template <typename DoubleType>
bool Preconditioner<DoubleType>::LUSolve(DoubleVec_t<DoubleType> &x, const DoubleVec_t<DoubleType> &b) const
{
#ifndef NDEBUG
  dsAssert(factored, "UNEXPECTED");
  dsAssert(static_cast<size_t>(b.size()) == size(), "UNEXPECTED");
#endif

  bool ret = false;

  FPECheck::ClearFPE();

  //// This should be able to return a value too
  this->DerivedLUSolve(x, b);

#if (defined(__arm64__) && defined(__APPLE__)) || defined(__aarch64__)
  FPECheck::ClearFPE();
#endif

  if (FPECheck::CheckFPE())
  {
    std::ostringstream os;
    os << "There was a floating point exception of type \"" << FPECheck::getFPEString() << "\"  during LU Back Substitution\n";
    OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
    FPECheck::ClearFPE();
  }
  else
  {
    ret = true;
  }

  return ret;
}

template <typename DoubleType>
bool Preconditioner<DoubleType>::LUSolve(ComplexDoubleVec_t<DoubleType> &x, const ComplexDoubleVec_t<DoubleType> &b) const
{
#ifndef NDEBUG
  dsAssert(factored, "UNEXPECTED");
  dsAssert(static_cast<size_t>(b.size()) == size(), "UNEXPECTED");
#endif

  bool ret = false;

  //// This should be able to return a value too
  this->DerivedLUSolve(x, b);

#if (defined(__arm64__) && defined(__APPLE__)) || defined(__aarch64__)
  FPECheck::ClearFPE();
#endif

  if (FPECheck::CheckFPE())
  {
    std::ostringstream os;
    os << "There was a floating point exception of type \"" << FPECheck::getFPEString() << "\"  during LU Back Substitution\n";
    OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
    FPECheck::ClearFPE();
  }
  else
  {
    ret = true;
  }

  return ret;
}
}

template class dsMath::Preconditioner<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class dsMath::Preconditioner<float128>;
#endif

