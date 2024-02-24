/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef DS_MATRIX_HH
#define DS_MATRIX_HH

#include "dsMathTypes.hh"

#include<utility>
#include<map>
#include<vector>


namespace dsMath {
template <typename DoubleType>
class Matrix {
    public:
//      virtual void AddSymbolic(int, int) = 0;  // add row,column to element list

        virtual void AddEntry(int, int, DoubleType) = 0;  // add row,column, value

        virtual void AddEntry(int, int, ComplexDouble_t<DoubleType>) = 0;

        virtual void AddImagEntry(int, int, DoubleType) = 0;  // add row,column, value

        virtual ~Matrix() = 0;

        inline size_t size() {return size_;}


        explicit Matrix(size_t);

        virtual void ClearMatrix() = 0;

        virtual void Finalize() = 0;

        virtual void Multiply(const DoubleVec_t<DoubleType> &/*x*/, DoubleVec_t<DoubleType> &/*y*/) const = 0;
        virtual void TransposeMultiply(const DoubleVec_t<DoubleType> &/*x*/, DoubleVec_t<DoubleType> &/*y*/) const = 0;
        virtual void Multiply(const ComplexDoubleVec_t<DoubleType> &/*x*/, ComplexDoubleVec_t<DoubleType> &/*y*/) const = 0;
        virtual void TransposeMultiply(const ComplexDoubleVec_t<DoubleType> &/*x*/, ComplexDoubleVec_t<DoubleType> &/*y*/) const = 0;

        DoubleVec_t<DoubleType> operator*(const DoubleVec_t<DoubleType> &x) const;
        ComplexDoubleVec_t<DoubleType> operator*(const ComplexDoubleVec_t<DoubleType> &x) const;

    protected:

    private:
        Matrix();
        Matrix(const Matrix &);
        Matrix &operator= (const Matrix &);

        size_t size_;
};
}
#endif

