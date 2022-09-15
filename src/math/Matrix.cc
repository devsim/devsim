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

#include "Matrix.hh"
#include "MatrixEntries.hh"
#include "OutputStream.hh"

#include "dsAssert.hh"
#include <sstream>
#include <utility>

namespace dsMath {
template <typename DoubleType>
Matrix<DoubleType>::Matrix(size_t sz) : size_(sz) {}

template <typename DoubleType> Matrix<DoubleType>::~Matrix() {}

template <typename DoubleType>
DoubleVec_t<DoubleType>
Matrix<DoubleType>::operator*(const DoubleVec_t<DoubleType> &x) const {
  DoubleVec_t<DoubleType> y;
  this->Multiply(x, y);
  return y;
}

template <typename DoubleType>
ComplexDoubleVec_t<DoubleType>
Matrix<DoubleType>::operator*(const ComplexDoubleVec_t<DoubleType> &x) const {
  ComplexDoubleVec_t<DoubleType> y;
  this->Multiply(x, y);
  return y;
}
} // namespace dsMath

template class dsMath::Matrix<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class dsMath::Matrix<float128>;
#endif
