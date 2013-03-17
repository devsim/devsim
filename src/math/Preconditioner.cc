/***
DEVSIM
Copyright 2013 Devsim LLC

This file is part of DEVSIM.

DEVSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, version 3.

DEVSIM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with DEVSIM.  If not, see <http://www.gnu.org/licenses/>.
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
