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
