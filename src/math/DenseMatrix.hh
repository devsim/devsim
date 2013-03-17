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

#ifndef DENSE_MATRIX_HH
#define DENSE_MATRIX_HH
#include <vector>
#include <complex>
namespace dsMath {
template <typename T> class DenseMatrix;
typedef DenseMatrix<double> RealDenseMatrix;
typedef DenseMatrix<std::complex<double> > ComplexDenseMatrix;

template <typename T> class DenseMatrix {
  public:
    enum Type_t {REAL, COMPLEX};
    //// Assume square for now
    explicit DenseMatrix(size_t);

    T &operator()(size_t, size_t);
    T operator()(size_t, size_t) const;

    bool LUFactor();

    //// Done inplace
    bool Solve(std::vector<T> &);

  private:
    DenseMatrix &operator=(const DenseMatrix &);
    DenseMatrix(const DenseMatrix &);
    DenseMatrix();

    std::vector<T> A_;
    std::vector<int>    ipiv_;
    int                 dim_;
    bool                factored_;
    int                 info_;
    Type_t              type_;
    
};
}
#endif
