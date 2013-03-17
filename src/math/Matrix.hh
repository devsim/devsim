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

#ifndef DS_MATRIX_HH
#define DS_MATRIX_HH

#include "dsMathTypes.hh"

#include<utility>
#include<map>
#include<vector>


namespace dsMath {
class Matrix;
class Matrix {
    public:
//      virtual void AddSymbolic(int, int) = 0;  // add row,column to element list

        virtual void AddEntry(int, int, double) = 0;  // add row,column, value

        virtual void AddEntry(int, int, ComplexDouble_t) = 0;

        virtual void AddImagEntry(int, int, double) = 0;  // add row,column, value

        virtual ~Matrix() = 0;

        inline size_t size() {return size_;}


        explicit Matrix(size_t);

        virtual void ClearMatrix() = 0;

        virtual void Finalize() = 0;

        virtual void Multiply(const DoubleVec_t &/*x*/, DoubleVec_t &/*y*/) const = 0;
        virtual void TransposeMultiply(const DoubleVec_t &/*x*/, DoubleVec_t &/*y*/) const = 0;
        virtual void Multiply(const ComplexDoubleVec_t &/*x*/, ComplexDoubleVec_t &/*y*/) const = 0;
        virtual void TransposeMultiply(const ComplexDoubleVec_t &/*x*/, ComplexDoubleVec_t &/*y*/) const = 0;

        DoubleVec_t operator*(const DoubleVec_t &x) const;
        ComplexDoubleVec_t operator*(const ComplexDoubleVec_t &x) const;

    protected:

    private:
        Matrix();
        Matrix(const Matrix &);
        Matrix &operator= (const Matrix &);

        size_t size_;
};
}
#endif
