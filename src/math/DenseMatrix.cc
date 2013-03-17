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

  if (sizeof(T) == sizeof(double))
  {
    type_ = REAL;
  }
  else if (sizeof(T) == sizeof(doublecomplex))
  {
    type_ = COMPLEX;
  }
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

template <typename T>
bool DenseMatrix<T>::LUFactor()
{
  if (type_ == REAL)
  {
    dgetrf(&dim_, &dim_, reinterpret_cast<double *>(&A_[0]), &dim_, &ipiv_[0], &info_);
  }
  else if (type_ == COMPLEX)
  {
    zgetrf(&dim_, &dim_, reinterpret_cast<doublecomplex *>(&A_[0]), &dim_, &ipiv_[0], &info_);
  }
  factored_ = true;
  return (info_ == 0);
}

template <typename T>
bool DenseMatrix<T>::Solve(std::vector<T> &B)
{
  static char trans = 'N';
  static int  nrhs   = 1;
//  assert(factored_);
//  assert(B.size() == size_t(dim_));
  if (info_ == 0)
  {
    if (type_ == REAL)
    {
#ifdef WIN32
      dgetrs(&trans, &dim_, &nrhs, reinterpret_cast<double *>(&A_[0]), &dim_, &ipiv_[0], reinterpret_cast<double *>(&B[0]), &dim_, &info_, 1);
#else
      dgetrs(&trans, &dim_, &nrhs, reinterpret_cast<double *>(&A_[0]), &dim_, &ipiv_[0], reinterpret_cast<double *>(&B[0]), &dim_, &info_);
#endif
    }
    else if (type_ == COMPLEX)
    {
#ifdef WIN32
      zgetrs(&trans, &dim_, &nrhs, reinterpret_cast<doublecomplex *>(&A_[0]), &dim_, &ipiv_[0], reinterpret_cast<doublecomplex *>(&B[0]), &dim_, &info_, 1);
#else
      zgetrs(&trans, &dim_, &nrhs, reinterpret_cast<doublecomplex *>(&A_[0]), &dim_, &ipiv_[0], reinterpret_cast<doublecomplex *>(&B[0]), &dim_, &info_);
#endif
    }
  }
  return (info_ == 0);
}

//// Manual Template Instantiation
template class DenseMatrix<double>;
template class DenseMatrix<std::complex<double> >;

}

