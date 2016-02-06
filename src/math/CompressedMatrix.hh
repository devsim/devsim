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

#ifndef DS_COMPRESSED_MATRIX_HH
#define DS_COMPRESSED_MATRIX_HH

#include "Matrix.hh"
#include "dsMathTypes.hh"

#include<utility>
#include<map>
#include<vector>
#include<unordered_map>

namespace dsMath {
class Matrix;
class CompressedMatrix : public Matrix {
    public:

        enum MatrixType {REAL, COMPLEX};
        ///// CCM is compressed column, CRM is compressed row
        enum CompressionType {CCM, CRM};
        enum SymbolicStatus_t {NEW_SYMBOLIC, SAME_SYMBOLIC};

        typedef std::unordered_map<int, int >         RowInd;
        typedef std::pair<int, int >        RowColEntry;
        typedef std::vector<RowColEntry>    RowColEntryVec;
        typedef std::vector<RowInd >        SymbolicMat;
        typedef std::map<int, double>       ColValueEntry;
        typedef std::vector<ColValueEntry > RowColValueEntries;

        void AddSymbolic(int, int);  // add row,column to element list

        void AddEntry(int, int, double);  // add row,column, value

        void AddEntry(int, int, ComplexDouble_t);

        void AddImagEntry(int, int, double);  // add row,column, value


        void ClearMatrix(); // zero the elements so that we can start the next iteration
        virtual ~CompressedMatrix();

        inline const MatrixType &GetMatrixType() const {return matType_;}

        CompressionType GetCompressionType() const {
                                    return compressionType_;
                                  }

        SymbolicStatus_t GetSymbolicStatus() const {
                                    return symbolicstatus_;
                                  }

        /// This is compressed column format
        const IntVec_t           &GetCols() const;
        const IntVec_t           &GetRows() const;
        const DoubleVec_t        &GetReal() const;
        const DoubleVec_t        &GetImag() const;
        const ComplexDoubleVec_t GetComplex() const;

        explicit CompressedMatrix(size_t, MatrixType = REAL, CompressionType = CCM);

        void Finalize();

        void Multiply(const DoubleVec_t &/*x*/, DoubleVec_t &/*y*/) const;
        void TransposeMultiply(const DoubleVec_t &/*x*/, DoubleVec_t &/*y*/) const;
        void Multiply(const ComplexDoubleVec_t &/*x*/, ComplexDoubleVec_t &/*y*/) const;
        void TransposeMultiply(const ComplexDoubleVec_t &/*x*/, ComplexDoubleVec_t &/*y*/) const;

    protected:
        void CreateMatrix(); // Create compressed columns

        void SetCompressed(bool);
        bool GetCompressed();
        
        void DecompressMatrix();

    private:
        CompressedMatrix();
        // Make sure that we copy all aspects(including pointers) later on
        CompressedMatrix(const Matrix &);
        CompressedMatrix &operator= (const Matrix &);

        MatrixType      matType_;
        CompressionType compressionType_;

        //// These should be properties of a compressed column matrix, just not a regular matrix
        SymbolicMat         Symbolic_;
        RowColValueEntries  OutOfBandEntries_Real;
        RowColValueEntries  OutOfBandEntries_Imag;
        IntVec_t    Ap_;
        IntVec_t    Ai_;
        //// Should these just be one complex vector
        DoubleVec_t Ax_;
        DoubleVec_t Az_;
        bool compressed;
        SymbolicStatus_t symbolicstatus_;
};

}
#endif
