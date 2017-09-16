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

#include "DenseMatrix.hh"
#include "BlasHeaders.hh"
#include "dsAssert.hh"
#include <complex>

namespace dsMath {
template <typename T>
DenseMatrix<T>::DenseMatrix(size_t d) : dim_(d), factored_(false), info_(0)
{
  A_.resize(dim_*dim_);
  ipiv_.resize(dim_);
}

template <typename T>
T &DenseMatrix<T>::operator()(size_t r, size_t c)
{
  return A_[r + c * dim_];
}

template <typename T>
T DenseMatrix<T>::operator()(size_t r, size_t c) const
{
  return A_[r + c * dim_];
}

template <typename DoubleType>
bool DenseMatrix<DoubleType>::LUFactor()
{
  getrf(&dim_, &dim_, reinterpret_cast<DoubleType *>(&A_[0]), &dim_, &ipiv_[0], &info_);
  factored_ = true;
  return (info_ == 0);
}

template <typename DoubleType>
bool DenseMatrix<DoubleType>::Solve(std::vector<DoubleType> &B)
{
  static char trans = 'N';
  static int  nrhs   = 1;

  if (info_ == 0)
  {
    getrs(&trans, &dim_, &nrhs, reinterpret_cast<DoubleType *>(&A_[0]), &dim_, &ipiv_[0], reinterpret_cast<DoubleType *>(&B[0]), &dim_, &info_);
  }
  return (info_ == 0);
}

}

//// Manual Template Instantiation
template class dsMath::DenseMatrix<double>;
#if 0
template class dsMath::DenseMatrix<std::complex<double> >;
#endif
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class dsMath::DenseMatrix<float128>;
#if 0
template class dsMath::DenseMatrix<std::complex<float128> >;
#endif
#endif

