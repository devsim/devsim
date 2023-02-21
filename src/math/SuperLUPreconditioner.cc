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

#include "SuperLUPreconditioner.hh"
#include "SuperLUData.hh"
#include "CompressedMatrix.hh"
#include "dsAssert.hh"
#include <utility>
#include <complex>


namespace dsMath {
template <typename DoubleType>
SuperLUPreconditioner<DoubleType>::SuperLUPreconditioner(size_t sz, PEnum::TransposeType_t transpose, PEnum::LUType_t lutype) : Preconditioner<DoubleType>(sz, transpose), superLUData_(nullptr), lutype_(lutype)
{
}

template <typename DoubleType>
dsMath::CompressionType SuperLUPreconditioner<DoubleType>::GetRealMatrixCompressionType() const
{
  return dsMath::CompressionType::CCM;
}

template <typename DoubleType>
dsMath::CompressionType SuperLUPreconditioner<DoubleType>::GetComplexMatrixCompressionType() const
{
  return dsMath::CompressionType::CCM;
}

template <typename DoubleType>
SuperLUPreconditioner<DoubleType>::~SuperLUPreconditioner()
{
  delete superLUData_;
}

template <typename DoubleType>
bool SuperLUPreconditioner<DoubleType>::DerivedLUFactor(Matrix<DoubleType> *m)
{
  CompressedMatrix<DoubleType> *cm = dynamic_cast<CompressedMatrix<DoubleType> *>(m);
  dsAssert(cm, "UNEXPECTED");
  dsAssert(cm->GetCompressionType() == CompressionType::CCM, "UNEXPECTED");

//  delete superLUData_;
  if (!superLUData_)
  {
    if (lutype_ == PEnum::LUType_t::FULL)
    {
      superLUData_ = new SuperLUData(Preconditioner<DoubleType>::size(), Preconditioner<DoubleType>::GetTransposeSolve(), PEnum::LUType_t::FULL);
    }
    else if (lutype_ == PEnum::LUType_t::INCOMPLETE)
    {
      superLUData_ = new SuperLUData(Preconditioner<DoubleType>::size(), Preconditioner<DoubleType>::GetTransposeSolve(), PEnum::LUType_t::INCOMPLETE);
    }
    else
    {
      dsAssert(0, "UNEXPECTED");
    }
  }

  bool ret = superLUData_->LUFactorMatrix(cm);
  return ret;
}

template <typename DoubleType>
void SuperLUPreconditioner<DoubleType>::DerivedLUSolve(DoubleVec_t<DoubleType> &x, const DoubleVec_t<DoubleType> &b) const
{
  superLUData_->LUSolve(x, b);
}

template <typename DoubleType>
void SuperLUPreconditioner<DoubleType>::DerivedLUSolve(ComplexDoubleVec_t<DoubleType> &x, const ComplexDoubleVec_t<DoubleType> &b) const
{
  superLUData_->LUSolve(x, b);
}
}


template class dsMath::SuperLUPreconditioner<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class dsMath::SuperLUPreconditioner<float128>;
#endif

