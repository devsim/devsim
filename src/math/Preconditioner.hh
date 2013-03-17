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

#ifndef PRECONDITIONER_HH
#define PRECONDITIONER_HH
#include "dsMathTypes.hh"
namespace dsMath {
class Matrix;
class Preconditioner {
  public:
    virtual ~Preconditioner() = 0;

    enum TransposeType_t {NOTRANS, TRANS};
    enum LUType_t {FULL, INCOMPLETE};

    Preconditioner(size_t /*numeqns*/, Preconditioner::TransposeType_t /*tranpose*/);
    bool LUFactor(Matrix *);     // Factor the matrix

    bool LUSolve(DoubleVec_t &x, const DoubleVec_t &b) const;
    bool LUSolve(ComplexDoubleVec_t &x, const ComplexDoubleVec_t &b) const;

    void SetTransposeSolve(bool);
    bool GetTransposeSolve();

    inline size_t size() const {return size_;}

  protected:
    virtual void DerivedLUSolve(DoubleVec_t &x, const DoubleVec_t &b) const =0;
    virtual void DerivedLUSolve(ComplexDoubleVec_t &x, const ComplexDoubleVec_t &b) const =0;
    virtual bool DerivedLUFactor(Matrix *)=0;     // Factor the matrix

    Matrix &GetMatrix()
    {
      return *matrix_;
    }

  private:
    size_t size_;
    bool factored;
    TransposeType_t transpose_solve_;
    Matrix *matrix_;

};
}
#endif
