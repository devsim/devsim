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

