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

#ifndef DS_SUPERLU_DATA
#define DS_SUPERLU_DATA
#include "dsMathTypes.hh"
#include "Preconditioner.hh"

///// SuperLU
typedef int int_t;
#include "supermatrix.h"

namespace dsMath {
class CompressedMatrix;
class SuperLUData {
  public:
    SuperLUData(size_t /*numeqns*/, bool /*transpose*/, Preconditioner::LUType_t /*lutype*/);
    ~SuperLUData();
    bool LUFactorMatrix(CompressedMatrix *);

    void LUSolve(DoubleVec_t &/*x*/, const DoubleVec_t &/*b*/);

    void LUSolve(ComplexDoubleVec_t &/*x*/, const ComplexDoubleVec_t &/*b*/);

    void DeleteStorage();

  protected:
    bool LUFactorRealMatrix(CompressedMatrix *);
    bool LUFactorComplexMatrix(CompressedMatrix *);

  private:    
    int          numeqns_;
    bool         transpose_;
    Preconditioner::LUType_t     lutype_;
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
