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

#include "DenseMatrix.hh"

#if defined(USE_EIGEN)
#include "Eigen/Dense"
#include "Eigen/LU"
#endif
#include "BlasHeaders.hh"

#include "dsAssert.hh"
#include <vector>

namespace dsMath {
template <typename T> struct matrix_data;

// blas/lapack implementation
template <typename T>
struct matrix_data_lapack {
    matrix_data_lapack(size_t d) : dim_(d), factored_(false), info_(0)
    {
      A_.resize(dim_*dim_);
      ipiv_.resize(dim_);
    }

    matrix_data_lapack() = delete;

    T & operator()(size_t r, size_t c)
    {
      // may worry about cache misses in the future between fortran and c
      // probably not important for our application since it is only done once and the LUFactors are stored
      return A_[r + c * dim_];
    }

    bool LUFactor()
    {
      getrf(&dim_, &dim_, reinterpret_cast<T *>(&A_[0]), &dim_, &ipiv_[0], &info_);
      factored_ = true;
      return (info_ == 0);
    }

    bool Solve(T *B)
    {
      static char trans = 'N';
      static int  nrhs   = 1;

      if (info_ == 0)
      {
        getrs(&trans, &dim_, &nrhs, reinterpret_cast<T *>(&A_[0]), &dim_, &ipiv_[0], B, &dim_, &info_);
      }
      return (info_ == 0);
    }

    std::vector<T> A_;
    std::vector<int>    ipiv_;
    int                 dim_;
    bool                factored_;
    int                 info_;
};

#if defined(USE_EIGEN)
template <typename T>
struct matrix_data_eigen {
    using matrix_type = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>;
    using vector_type = Eigen::Vector<T, Eigen::Dynamic>;
    using lu_type = Eigen::FullPivLU<Eigen::Ref<matrix_type>>;

    matrix_data_eigen(size_t d) : A_(d, d)
    {
      A_ = matrix_type::Zero(d, d);
      ok_ = false;
    }

    matrix_data_eigen() = delete;

    T & operator()(size_t r, size_t c)
    {
      return A_(r, c);
    }

    bool LUFactor()
    {
      if (!LU_)
      {
        LU_ = std::make_unique<lu_type>(A_);
      }
      else
      {
        LU_->compute(A_);
      }
      ok_ = LU_->isInvertible();
      return ok_;
    }

    bool Solve(T *B)
    {
      Eigen::Map<vector_type> v(B, LU_->rows());
      if (ok_)
      {
        LU_->solve(v);
      }
      return ok_;
    }

    matrix_type A_;
    std::unique_ptr<lu_type> LU_;
    bool ok_;
};
#endif

template <typename T>
DenseMatrix<T>::DenseMatrix(size_t d)
{
  matrixdata_ = std::make_unique<matrix_data<T>>(d);
}

template <typename T>
DenseMatrix<T>::~DenseMatrix() = default;

template <typename T>
T &DenseMatrix<T>::operator()(size_t r, size_t c)
{
  return matrixdata_->operator()(r, c);
}

template <typename T>
T DenseMatrix<T>::operator()(size_t r, size_t c) const
{
  return matrixdata_->operator()(r, c);
}

template <typename DoubleType>
bool DenseMatrix<DoubleType>::LUFactor()
{
  return matrixdata_->LUFactor();
}

template <typename DoubleType>
bool DenseMatrix<DoubleType>::Solve(DoubleType *B)
{
  return matrixdata_->Solve(B);
}
}

//// Manual Template Instantiation
template <> struct dsMath::matrix_data <double> : public matrix_data_lapack<double> {using matrix_data_lapack<double>::matrix_data_lapack;};
template class dsMath::DenseMatrix<double>;

// right now, eigen is for float 128 only
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
#if defined(USE_EIGEN)
template <> struct dsMath::matrix_data <float128> : public matrix_data_eigen<float128> {using matrix_data_eigen<float128>::matrix_data_eigen;};
#else
template <> struct dsMath::matrix_data <float128> : public matrix_data_lapack<float128> {using matrix_data_lapack<float128>::matrix_data_lapack;};
#endif
template class dsMath::DenseMatrix<float128>;
#endif

