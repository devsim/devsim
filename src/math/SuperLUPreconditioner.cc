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

#include "SuperLUPreconditioner.hh"
#include "SuperLUData.hh"
#include "CompressedMatrix.hh"
#include "dsAssert.hh"
#include <utility>
#include <complex>


namespace dsMath {
SuperLUPreconditioner::SuperLUPreconditioner(size_t sz, Preconditioner::TransposeType_t transpose, LUType_t lutype) : Preconditioner(sz, transpose), superLUData_(NULL), lutype_(lutype)
{
}

SuperLUPreconditioner::~SuperLUPreconditioner()
{
  delete superLUData_;
}

bool SuperLUPreconditioner::DerivedLUFactor(Matrix *m)
{
  CompressedMatrix *cm = dynamic_cast<CompressedMatrix *>(m);
  dsAssert(cm, "UNEXPECTED");
  dsAssert(cm->GetCompressionType() == CompressedMatrix::CCM, "UNEXPECTED");

//  delete superLUData_;
  if (!superLUData_)
  {
    if (lutype_ == FULL)
    {
      superLUData_ = new SuperLUData(size(), GetTransposeSolve(), FULL);
    }
    else if (lutype_ == INCOMPLETE)
    {
      superLUData_ = new SuperLUData(size(), GetTransposeSolve(), INCOMPLETE);
    }
    else
    {
      dsAssert(0, "UNEXPECTED");
    }
  }

  bool ret = superLUData_->LUFactorMatrix(cm);
  return ret;
}

void SuperLUPreconditioner::DerivedLUSolve(DoubleVec_t &x, const DoubleVec_t &b) const
{
  superLUData_->LUSolve(x, b);
}

void SuperLUPreconditioner::DerivedLUSolve(ComplexDoubleVec_t &x, const ComplexDoubleVec_t &b) const
{
  superLUData_->LUSolve(x, b);
}
}

