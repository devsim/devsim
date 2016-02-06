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

#include <sstream>
#include <utility>
#include "dsAssert.hh"

namespace dsMath {
Matrix::Matrix(size_t sz) : size_(sz)
{
}

Matrix::~Matrix()
{
}

DoubleVec_t Matrix::operator*(const DoubleVec_t &x) const
{
  DoubleVec_t y;
  this->Multiply(x, y);
  return y;
}

ComplexDoubleVec_t Matrix::operator*(const ComplexDoubleVec_t &x) const
{
  ComplexDoubleVec_t y;
  this->Multiply(x, y);
  return y;
}

}

