/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "Matrix.hh"
#include "MatrixEntries.hh"
#include "OutputStream.hh"

#include <sstream>
#include <utility>
#include "dsAssert.hh"

namespace dsMath {
template <typename DoubleType>
Matrix<DoubleType>::Matrix(size_t sz) : size_(sz)
{
}

template <typename DoubleType>
Matrix<DoubleType>::~Matrix()
{
}

template <typename DoubleType>
DoubleVec_t<DoubleType> Matrix<DoubleType>::operator*(const DoubleVec_t<DoubleType> &x) const
{
  DoubleVec_t<DoubleType> y;
  this->Multiply(x, y);
  return y;
}

template <typename DoubleType>
ComplexDoubleVec_t<DoubleType> Matrix<DoubleType>::operator*(const ComplexDoubleVec_t<DoubleType> &x) const
{
  ComplexDoubleVec_t<DoubleType> y;
  this->Multiply(x, y);
  return y;
}
}

template class dsMath::Matrix<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class dsMath::Matrix<float128>;
#endif

