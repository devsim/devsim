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

#ifndef DS_MATRIX_HH
#define DS_MATRIX_HH

#include "dsMathTypes.hh"

#include <map>
#include <utility>
#include <vector>

namespace dsMath {
template <typename DoubleType> class Matrix {
public:
  //      virtual void AddSymbolic(int, int) = 0;  // add row,column to element
  //      list

  virtual void AddEntry(int, int, DoubleType) = 0; // add row,column, value

  virtual void AddEntry(int, int, std::complex<DoubleType>) = 0;

  virtual void AddImagEntry(int, int, DoubleType) = 0; // add row,column, value

  virtual ~Matrix() = 0;

  inline size_t size() { return size_; }

  explicit Matrix(size_t);

  virtual void ClearMatrix() = 0;

  virtual void Finalize() = 0;

  virtual void Multiply(const DoubleVec_t<DoubleType> & /*x*/,
                        DoubleVec_t<DoubleType> & /*y*/) const = 0;
  virtual void TransposeMultiply(const DoubleVec_t<DoubleType> & /*x*/,
                                 DoubleVec_t<DoubleType> & /*y*/) const = 0;
  virtual void Multiply(const ComplexDoubleVec_t<DoubleType> & /*x*/,
                        ComplexDoubleVec_t<DoubleType> & /*y*/) const = 0;
  virtual void
  TransposeMultiply(const ComplexDoubleVec_t<DoubleType> & /*x*/,
                    ComplexDoubleVec_t<DoubleType> & /*y*/) const = 0;

  DoubleVec_t<DoubleType> operator*(const DoubleVec_t<DoubleType> &x) const;
  ComplexDoubleVec_t<DoubleType>
  operator*(const ComplexDoubleVec_t<DoubleType> &x) const;

protected:
private:
  Matrix();
  Matrix(const Matrix &);
  Matrix &operator=(const Matrix &);

  size_t size_;
};
} // namespace dsMath
#endif
