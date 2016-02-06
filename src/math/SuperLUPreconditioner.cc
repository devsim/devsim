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

