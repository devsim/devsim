/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef DENSE_MATRIX_HH
#define DENSE_MATRIX_HH
#include <memory>
namespace dsMath {
template <typename T> class DenseMatrix;

template <typename T>
using RealDenseMatrix = DenseMatrix<T>;

template <typename T>
struct matrix_data;

template <typename T> class DenseMatrix {
  public:

    //// Assume square for now
    explicit DenseMatrix(size_t);
    ~DenseMatrix();

    T &operator()(size_t, size_t);
    T operator()(size_t, size_t) const;

    bool LUFactor();

    //// Done inplace
    bool Solve(T *);

  private:
    DenseMatrix &operator=(const DenseMatrix &);
    DenseMatrix(const DenseMatrix &);
    DenseMatrix();

    std::unique_ptr<matrix_data<T>> matrixdata_;

};
}
#endif

