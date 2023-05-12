/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef PRECONDITIONER_HH
#define PRECONDITIONER_HH
#include "dsMathTypes.hh"
namespace dsMath {
template <typename DoubleType>
class Matrix;
enum class CompressionType;

namespace PEnum {
enum class TransposeType_t {NOTRANS, TRANS};
enum class LUType_t {FULL, INCOMPLETE};
}

template <typename DoubleType>
class Preconditioner {
  public:
    virtual ~Preconditioner() = 0;
    virtual dsMath::CompressionType GetRealMatrixCompressionType() const = 0;
    virtual dsMath::CompressionType GetComplexMatrixCompressionType() const = 0;

    Preconditioner(size_t /*numeqns*/, PEnum::TransposeType_t /*tranpose*/);
    bool LUFactor(Matrix<DoubleType> *);     // Factor the matrix

    bool LUSolve(DoubleVec_t<DoubleType> &x, const DoubleVec_t<DoubleType> &b) const;
    bool LUSolve(ComplexDoubleVec_t<DoubleType> &x, const ComplexDoubleVec_t<DoubleType> &b) const;

#if 0
    void SetTransposeSolve(bool);
#endif
    bool GetTransposeSolve();

    inline size_t size() const {return size_;}

  protected:
    virtual void DerivedLUSolve(DoubleVec_t<DoubleType> &x, const DoubleVec_t<DoubleType> &b) const =0;
    virtual void DerivedLUSolve(ComplexDoubleVec_t<DoubleType> &x, const ComplexDoubleVec_t<DoubleType> &b) const =0;
    virtual bool DerivedLUFactor(Matrix<DoubleType> *)=0;     // Factor the matrix

    Matrix<DoubleType> &GetMatrix()
    {
      return *matrix_;
    }

  private:
    size_t size_;
    bool factored;
    PEnum::TransposeType_t transpose_solve_;
    Matrix<DoubleType> *matrix_;

};
}
#endif

