/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef DS_COMPRESSED_MATRIX_HH
#define DS_COMPRESSED_MATRIX_HH

#include "Matrix.hh"
#include "dsMathTypes.hh"

#include<utility>
#include<map>
#include<vector>
#include<unordered_map>

#include <iosfwd>

namespace dsMath {

enum class MatrixType {REAL, COMPLEX};
///// CCM is compressed column, CRM is compressed row
enum class CompressionType {CCM, CRM};
enum class SymbolicStatus_t {NEW_SYMBOLIC, SAME_SYMBOLIC};

template <typename DoubleType>
class CompressedMatrix : public Matrix<DoubleType> {
    public:

        typedef std::unordered_map<int, int >         RowInd;
        typedef std::pair<int, int >        RowColEntry;
        typedef std::vector<RowColEntry>    RowColEntryVec;
        typedef std::vector<RowInd >        SymbolicMat;
        typedef std::map<int, DoubleType>       ColValueEntry;
        typedef std::vector<ColValueEntry > RowColValueEntries;

        void AddEntry(int, int, DoubleType);  // add row,column, value

        void AddEntry(int, int, ComplexDouble_t<DoubleType>);

        void AddImagEntry(int, int, DoubleType);  // add row,column, value


        void ClearMatrix(); // zero the elements so that we can start the next iteration
        virtual ~CompressedMatrix();

        inline const MatrixType &GetMatrixType() const {return matType_;}

        CompressionType GetCompressionType() const {
                                    return compressionType_;
                                  }

        void SetSymbolicStatus(SymbolicStatus_t x)
        {
          symbolicstatus_ = x;
        }

        SymbolicStatus_t GetSymbolicStatus() const {
                                    return symbolicstatus_;
                                  }

        /// This is compressed column format
        const IntVec_t                            &GetAp() const {return Ap_;}
        const IntVec_t                            &GetAi() const {return Ai_;}
        const DoubleVec_t<DoubleType>             &GetAx() const {return Ax_;}
        const DoubleVec_t<DoubleType>             &GetAz() const {return Az_;}
        const IntVec_t                            &GetCols() const;
        const IntVec_t                            &GetRows() const;
        const DoubleVec_t<DoubleType>             &GetReal() const;
        const DoubleVec_t<DoubleType>             &GetImag() const;
        const ComplexDoubleVec_t<DoubleType>      &GetComplex() const;

        CompressedMatrix(size_t, MatrixType, CompressionType);

        void Finalize();

        void Multiply(const DoubleVec_t<DoubleType> &/*x*/, DoubleVec_t<DoubleType> &/*y*/) const;
        void TransposeMultiply(const DoubleVec_t<DoubleType> &/*x*/, DoubleVec_t<DoubleType> &/*y*/) const;
        void Multiply(const ComplexDoubleVec_t<DoubleType> &/*x*/, ComplexDoubleVec_t<DoubleType> &/*y*/) const;
        void TransposeMultiply(const ComplexDoubleVec_t<DoubleType> &/*x*/, ComplexDoubleVec_t<DoubleType> &/*y*/) const;

        void DebugMatrix(std::ostream &) const;

    protected:
        void CreateMatrix(); // Create compressed columns

        void SetCompressed(bool);
        bool GetCompressed();

        void DecompressMatrix();

    private:
        void AddSymbolicImpl(int, int);  // add row,column to element list
        void AddEntryImpl(int, int, DoubleType);  // add row,column, value
        //void AddEntryImpl(int, int, ComplexDouble_t<DoubleType>);
        void AddImagEntryImpl(int, int, DoubleType);  // add row,column, value

        CompressedMatrix();
        // Make sure that we copy all aspects(including pointers) later on
        CompressedMatrix(const CompressedMatrix<DoubleType> &);
        CompressedMatrix &operator= (const CompressedMatrix<DoubleType> &);

        MatrixType      matType_;
        CompressionType compressionType_;

        //// These should be properties of a compressed column matrix, just not a regular matrix
        SymbolicMat         Symbolic_;
        RowColValueEntries  OutOfBandEntries_Real;
        RowColValueEntries  OutOfBandEntries_Imag;
        IntVec_t    Ap_;
        IntVec_t    Ai_;
        //// Should these just be one complex vector
        DoubleVec_t<DoubleType> Ax_;
        DoubleVec_t<DoubleType> Az_;
        mutable ComplexDoubleVec_t<DoubleType> Axz_;
        bool compressed;
        SymbolicStatus_t symbolicstatus_;

        static const inline DoubleType DTZERO{};
};

}
#endif
