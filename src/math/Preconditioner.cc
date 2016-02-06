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
Preconditioner::~Preconditioner()
{
}

Preconditioner::Preconditioner(size_t numeqns, Preconditioner::TransposeType_t transpose) : size_(numeqns), factored(false), transpose_solve_(transpose), matrix_(NULL)
{
}

#if 0
void Preconditioner::SetMatrix(Matrix *m)
{
  matrix_ = m;
  DerivedSetMatrix();
  factored = false;
}
#endif

void Preconditioner::SetTransposeSolve(bool x)
{
  if (x)
  {
    transpose_solve_ = Preconditioner::TRANS;
  }
  else
  {
    transpose_solve_ = Preconditioner::NOTRANS;
  }
}

bool Preconditioner::GetTransposeSolve()
{
  return (transpose_solve_ == Preconditioner::TRANS);
}

bool Preconditioner::LUFactor(Matrix *mat)
{

  factored = false;
  matrix_ = mat;

  FPECheck::ClearFPE();

  bool ret = this->DerivedLUFactor(matrix_);

  if (FPECheck::CheckFPE())
  {
    std::ostringstream os;
    os << "There was a floating point exception of type \"" << FPECheck::getFPEString() << "\"  during LU Factorization\n";
    OutputStream::WriteOut(OutputStream::FATAL, os.str().c_str());
    FPECheck::ClearFPE();
  }

  factored = ret;
  return ret;
}


bool Preconditioner::LUSolve(DoubleVec_t &x, const DoubleVec_t &b) const
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
    OutputStream::WriteOut(OutputStream::INFO, os.str());
    FPECheck::ClearFPE();
  }
  else
  {
    ret = true;
  }

  return ret;
}

bool Preconditioner::LUSolve(ComplexDoubleVec_t &x, const ComplexDoubleVec_t &b) const
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
    OutputStream::WriteOut(OutputStream::INFO, os.str());
    FPECheck::ClearFPE();
  }
  else
  {
    ret = true;
  }

  return ret;
}


}
