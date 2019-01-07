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

#if 0
template <typename DoubleType>
void Preconditioner<DoubleType>::SetMatrix(Matrix *m)
{
  matrix_ = m;
  DerivedSetMatrix();
  factored = false;
}
#endif

#if 0
template <typename DoubleType>
void Preconditioner<DoubleType>::SetTransposeSolve(bool x)
{
  if (x)
  {
    transpose_solve_ = PEnum::TransposeType_t::TRANS;
  }
  else
  {
    transpose_solve_ = PEnum::TransposeType_t::NOTRANS;
  }
}
#endif

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

  if (FPECheck::CheckFPE())
  {
    std::ostringstream os;
    os << "There was a floating point exception of type \"" << FPECheck::getFPEString() << "\"  during LU Factorization\n";
    OutputStream::WriteOut(OutputStream::OutputType::FATAL, os.str().c_str());
    FPECheck::ClearFPE();
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

