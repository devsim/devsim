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
