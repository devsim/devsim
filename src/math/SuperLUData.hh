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

#ifndef DS_SUPERLU_DATA
#define DS_SUPERLU_DATA
#include "dsMathTypes.hh"
#include "Preconditioner.hh"

///// SuperLU
typedef int int_t;
#include "supermatrix.h"

namespace dsMath {
template <typename DoubleType>
class CompressedMatrix;

class SuperLUData {
  public:
    SuperLUData(size_t /*numeqns*/, bool /*transpose*/, PEnum::LUType_t /*lutype*/);
    ~SuperLUData();

    template <typename DoubleType>
    bool LUFactorMatrix(CompressedMatrix<DoubleType> *);

    template <typename DoubleType>
    void LUSolve(DoubleVec_t<DoubleType> &/*x*/, const DoubleVec_t<DoubleType> &/*b*/);

    template <typename DoubleType>
    void LUSolve(ComplexDoubleVec_t<DoubleType> &/*x*/, const ComplexDoubleVec_t<DoubleType> &/*b*/);

    void DeleteStorage();

  protected:
    template <typename DoubleType>
    bool LUFactorRealMatrix(CompressedMatrix<DoubleType> *, const DoubleVec_t<double> &);
    template <typename DoubleType>
    bool LUFactorComplexMatrix(CompressedMatrix<DoubleType> *, const ComplexDoubleVec_t<double> &);

  private:
    int          numeqns_;
    bool         transpose_;
    PEnum::LUType_t     lutype_;
    int         *perm_r_;
    int         *perm_c_;
    int         *etree_;
    SuperMatrix *L_;
    SuperMatrix *U_;
    int          info_;
};

#if 0
int check_perm(const int n, const int *perm);
#endif
}
#endif

