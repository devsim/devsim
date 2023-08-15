/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "CompressedMatrix.hh"
#include "MatrixEntries.hh"
#include "OutputStream.hh"
#include "dsAssert.hh"

#include <sstream>
#include <utility>
#include <algorithm>

#include <iomanip>

namespace dsMath {

template <typename DoubleType>
void CompressedMatrix<DoubleType>::DebugMatrix(std::ostream &os) const
{
  if (matType_ == MatrixType::COMPLEX)
  {
    os << "#COMPLEX\n";
  }
  else
  {
    os << "#REAL\n";
  }

  if (compressionType_ == CompressionType::CCM)
  {
    os << "CCM";
  }
  else
  {
    os << "CRM";
  }

  os << " = {\n";

  os << "'ia' : [\n";
  for (size_t i = 0; i < Ap_.size(); ++i)
  {
    os << Ap_[i] << ",\n";
  }
  os << "],\n";

  os << "'ja' : [\n";
  for (size_t i = 0; i < Ai_.size(); ++i)
  {
    os << Ai_[i] << ",\n";
  }
  os << "],\n";

  os << "'a' : [\n";

  std::ostringstream fos;
  fos << std::setprecision(15);
  if (matType_ == MatrixType::REAL)
  {
    for (size_t i = 0; i < Ax_.size(); ++i)
    {
      fos << Ax_[i] << ",\n";
    }
  }
  else
  {
    for (size_t i = 0; i < Ax_.size(); ++i)
    {
      fos << "complex(" << Ax_[i]  << "," << Az_[i] << "),\n";
    }
  }
  os << fos.str();
  os << "],\n";
  os << "}\n";
}

template <typename DoubleType>
CompressedMatrix<DoubleType>::CompressedMatrix(size_t sz, MatrixType mt, CompressionType ct) : Matrix<DoubleType>(sz), matType_(mt), compressionType_(ct), compressed(false), symbolicstatus_(SymbolicStatus_t::NEW_SYMBOLIC)
{
  Symbolic_.resize(this->size());
  OutOfBandEntries_Real.resize(this->size());
  if (GetMatrixType() == MatrixType::COMPLEX)
  {
    OutOfBandEntries_Imag.resize(this->size());
  }
}

template <typename DoubleType>
CompressedMatrix<DoubleType>::~CompressedMatrix()
{
}

/*
*/
template <typename DoubleType>
void CompressedMatrix<DoubleType>::AddSymbolicImpl(int r, int c)
{
  /// remember compressed column
  /// need to make it possible to add out of band entries
#ifndef NDEBUG
  dsAssert(!compressed, "UNEXPECTED");
#endif
  Symbolic_[c].insert(std::make_pair(r, 0)); // should create implicit entry
}

/// Create Matrix from symbolic info
template <typename DoubleType>
void CompressedMatrix<DoubleType>::CreateMatrix()
{
  const size_t cols = Symbolic_.size();
#ifndef NDEBUG
  dsAssert(cols == this->size(), "UNEXPECTED");
#endif

  Ap_.resize(this->size()+1);
  Ai_.clear();
  Ai_.reserve(this->size());

  int r=0;
  for (size_t c=0; c < cols;  ++c)
  {
    Ap_[c]=r;

    RowInd &rivec = Symbolic_[c];
    RowInd::iterator iter = rivec.begin();
    RowInd::iterator iend = rivec.end();

    int rb = r;

    for (; iter != iend; ++iter)
    {
      Ai_.push_back((*iter).first);
      ++r;
    }

    if (r > rb)
    {
      IntVec_t::iterator abegin = Ai_.begin();
      std::advance(abegin, rb);
      std::sort(abegin, Ai_.end());
    }

    for (int i = rb; i < r; ++i)
    {
      rivec[Ai_[i]] = i;
//        (*iter).second = r; // gives us the position in Ai,Ax
    }
  }
  Ap_[this->size()] = r;

  // Ye old swap trick
  IntVec_t(Ai_).swap(Ai_);
  // reserve room
  Ax_.clear();
  Ax_.resize(Ai_.size());
  if (GetMatrixType() == MatrixType::COMPLEX) {
    Az_.clear();
    Az_.resize(Ai_.size());
  }

  compressed = true;
}

template <typename DoubleType>
void CompressedMatrix<DoubleType>::SetCompressed(bool x)
{
    compressed = true;
}

template <typename DoubleType>
bool CompressedMatrix<DoubleType>::GetCompressed()
{
    return compressed;
}

template <typename DoubleType>
const dsMath::IntVec_t &CompressedMatrix<DoubleType>::GetCols() const
{
  dsAssert(compressed, "UNEXPECTED");

  if (compressionType_ == CompressionType::CCM)
  {
    return Ap_;
  }
  else if (compressionType_ == CompressionType::CRM)
  {
    return Ai_;
  }
  else
  {
    dsAssert(0, "UNEXPECTED");
  }

  return Ap_;
}

template <typename DoubleType>
const dsMath::IntVec_t &CompressedMatrix<DoubleType>::GetRows() const
{
  dsAssert(compressed, "UNEXPECTED");

  if (compressionType_ == CompressionType::CCM)
  {
    return Ai_;
  }
  else if (compressionType_ == CompressionType::CRM)
  {
    return Ap_;
  }
  else
  {
    dsAssert(0, "UNEXPECTED");
  }
  return Ai_;
}

template <typename DoubleType>
const dsMath::DoubleVec_t<DoubleType> &CompressedMatrix<DoubleType>::GetReal() const
{
  dsAssert(compressed, "UNEXPECTED");

  return Ax_;
}

template <typename DoubleType>
const dsMath::DoubleVec_t<DoubleType> &CompressedMatrix<DoubleType>::GetImag() const
{
  dsAssert(compressed, "UNEXPECTED");

  return Az_;
}

template <typename DoubleType>
const dsMath::ComplexDoubleVec_t<DoubleType> &CompressedMatrix<DoubleType>::GetComplex() const
{
  dsAssert(compressed, "UNEXPECTED");
  dsAssert(Ax_.size() == Az_.size(), "UNEXPECTED");

  size_t len = Ax_.size();

  Axz_.resize(len);

  for (size_t i = 0; i < len; ++i)
  {
    Axz_[i].real(Ax_[i]);
  }

  for (size_t i = 0; i < len; ++i)
  {
    Axz_[i].imag(Az_[i]);
  }

  return Axz_;
}

template <typename DoubleType>
void CompressedMatrix<DoubleType>::AddEntryImpl(int r, int c, DoubleType v)
{
#ifndef NDEBUG
  dsAssert(static_cast<size_t>(r) < this->size(), "UNEXPECTED");
  dsAssert(static_cast<size_t>(c) < this->size(), "UNEXPECTED");
#endif

  if (v == DTZERO)
  {
    return;
  }

  if (compressed)
  {
    /// need to adapt for handling of out of band entries
    /// and recompressing matrix
    const RowInd &ri = Symbolic_[c];
    RowInd::const_iterator rit = ri.find(r);
    if (rit != ri.end())
    {
      Ax_[rit->second] += v;
    }
    else
    {
      DecompressMatrix();
      AddSymbolicImpl(r, c);
      OutOfBandEntries_Real[r][c] += v;
    }
  }
  else
  {
    AddSymbolicImpl(r, c);
    /// the double entry is initialized to zero (property of map)
    OutOfBandEntries_Real[r][c] += v;
  }
}

template <typename DoubleType>
void CompressedMatrix<DoubleType>::AddEntry(int r, int c, DoubleType v)
{
  if (compressionType_ == CompressionType::CRM)
  {
    AddEntryImpl(c, r, v);
  }
  else
  {
    AddEntryImpl(r, c, v);
  }
}

/// Model this after
template <typename DoubleType>
void CompressedMatrix<DoubleType>::AddImagEntryImpl(int r, int c, DoubleType v)
{
#ifndef NDEBUG
  dsAssert(GetMatrixType() == MatrixType::COMPLEX, "UNEXPECTED");
  dsAssert(static_cast<size_t>(r) < this->size(), "UNEXPECTED");
  dsAssert(static_cast<size_t>(c) < this->size(), "UNEXPECTED");
#endif

  if (v == DTZERO)
  {
    return;
  }

  if (compressed)
  {
    const RowInd &ri = Symbolic_[c];
    RowInd::const_iterator rit = ri.find(r);
    if (rit != ri.end())
    {
      Az_[rit->second] += v;
    }
    else
    {
      DecompressMatrix();
      AddSymbolicImpl(r, c);
      OutOfBandEntries_Imag[r][c] += v;
    }
  }
  else
  {
    AddSymbolicImpl(r, c);
    /// the double entry is initialized to zero (property of map)
    OutOfBandEntries_Imag[r][c] += v;
  }
}

template <typename DoubleType>
void CompressedMatrix<DoubleType>::AddImagEntry(int r, int c, DoubleType v)
{
  if (compressionType_ == CompressionType::CRM)
  {
    AddImagEntryImpl(c, r, v);
  }
  else
  {
    AddImagEntryImpl(r, c, v);
  }
}


template <typename DoubleType>
void CompressedMatrix<DoubleType>::AddEntry(int r, int c, ComplexDouble_t<DoubleType> v)
{
  const auto &rv = v.real();
  const auto &iv = v.imag();

  if (rv != DTZERO)
  {
    AddEntry(r, c, rv);
  }

  if (iv != DTZERO)
  {
    AddImagEntry(r, c, iv);
  }
}

template <typename DoubleType>
void CompressedMatrix<DoubleType>::DecompressMatrix()
{
  if (!compressed)
  {
    return;
  }
  std::ostringstream os;
  os << "Matrix Decompress!!! Symbolic pattern changed\n";
  OutputStream::WriteOut(OutputStream::OutputType::VERBOSE1, os.str());
  compressed = false;
  const size_t sz = Ap_.size() - 1;

#ifndef NDEBUG
  dsAssert(sz == this->size(), "UNEXPECTED");
#endif

  /// size is 1 past in compressed column
  for (size_t i = 0; i < sz; ++i)
  {
    const size_t beg = Ap_[i];
    const size_t end = Ap_[i+1];
    for (size_t j = beg; j < end; ++ j)
    {
      AddEntryImpl(Ai_[j], i, Ax_[j]);
    }
    if (GetMatrixType() == MatrixType::COMPLEX)
    {
      for (size_t j = beg; j < end; ++ j)
      {
        const auto &z = Az_[j];
        if (z != DTZERO)
        {
          AddImagEntryImpl(Ai_[j], i, z);
        }
      }
    }
  }
  Ai_.clear();
  Ax_.clear();
  Az_.clear();
}


template <typename DoubleType>
void CompressedMatrix<DoubleType>::Finalize()
{
  if (!compressed)
  {
    symbolicstatus_ = SymbolicStatus_t::NEW_SYMBOLIC;

    CreateMatrix();
    for (size_t i = 0; i < OutOfBandEntries_Real.size(); ++i)
    {
      typename ColValueEntry::iterator it = OutOfBandEntries_Real[i].begin();
      const typename ColValueEntry::iterator itend = OutOfBandEntries_Real[i].end();
      for ( ; it != itend; ++it)
      {
        AddEntryImpl(i, it->first, it->second);
      }
    }
    OutOfBandEntries_Real.clear();
    OutOfBandEntries_Real.resize(this->size());

    if (GetMatrixType() == MatrixType::COMPLEX)
    {
      for (size_t i = 0; i < OutOfBandEntries_Imag.size(); ++i)
      {
        typename ColValueEntry::iterator it = OutOfBandEntries_Imag[i].begin();
        const typename ColValueEntry::iterator itend = OutOfBandEntries_Imag[i].end();
        for ( ; it != itend; ++it)
        {
          AddImagEntryImpl(i, it->first, it->second);
        }
      }
      OutOfBandEntries_Imag.clear();
      OutOfBandEntries_Imag.resize(this->size());
    }
  }
  else
  {
    symbolicstatus_ = SymbolicStatus_t::SAME_SYMBOLIC;
  }
}

template <typename DoubleType>
void CompressedMatrix<DoubleType>::ClearMatrix()
{
//  compressed = false;
  const size_t sz = Ax_.size();
  Ax_.clear();
  Ax_.resize(sz);
  if (GetMatrixType() == MatrixType::COMPLEX)
  {
    Az_.clear();
    Az_.resize(sz);
  }

  OutOfBandEntries_Real.clear();
  OutOfBandEntries_Real.resize(sz);
  if (GetMatrixType() == MatrixType::COMPLEX)
  {
    OutOfBandEntries_Imag.clear();
    OutOfBandEntries_Imag.resize(sz);
  }
}

namespace {
//// Written in terms of compressed column format, note that cols is 1 + the size of the matrix
//// looks like scaling each column by a constant factor
template <typename T>
void ColScaleMultiply(const IntVec_t &cols, const IntVec_t &rows, const std::vector<T> &vals, const std::vector<T> &x, std::vector<T> &y)
{
  y.clear();
  /// zeroes the entries
  y.resize(x.size());

  const size_t size = cols.size() - 1;
  for (size_t c = 0; c < size; ++c)
  {
    const T scale = x[c];

    const size_t rl = cols[c];
    const size_t rh = cols[c+1];

    for (size_t rit = rl; rit < rh; ++rit)
    {
      const size_t r = rows[rit];
      const T v = vals[rit];
      y[r] += scale * v;
    }
  }
}

//// Written in terms of compressed rows format, note that rows is 1 + the size of the matrix
//// This looks kind of like a dot product
template <typename T>
void RowScaleMultiply(const IntVec_t &rows, const IntVec_t &cols, const std::vector<T> &vals, const std::vector<T> &x, std::vector<T> &y)
{
  y.clear();
  /// zeroes the entries
  y.resize(x.size());

  const size_t size = rows.size() - 1;
  for (size_t r = 0; r < size; ++r)
  {
    const size_t cl = rows[r];
    const size_t ch = rows[r+1];

    T &outval = y[r];

    for (size_t cit = cl; cit < ch; ++cit)
    {
      const size_t c  = cols[cit];
      const T v  = vals[cit];
      const T xv = x[c];
      outval += v * xv;
    }
  }
}
}

template <typename DoubleType>
void CompressedMatrix<DoubleType>::Multiply(const DoubleVec_t<DoubleType> &x, DoubleVec_t<DoubleType> &y) const
{
  dsAssert(compressed, "UNEXPECTED");

  const IntVec_t    &Cols = this->GetCols();
  const IntVec_t    &Rows = this->GetRows();
  const DoubleVec_t<DoubleType> &Vals = this->GetReal();
  if (compressionType_ == CompressionType::CCM)
  {
    ColScaleMultiply(Cols, Rows, Vals, x, y);
  }
  else if (compressionType_ == CompressionType::CRM)
  {
    RowScaleMultiply(Rows, Cols, Vals, x, y);
  }
}

template <typename DoubleType>
void CompressedMatrix<DoubleType>::TransposeMultiply(const DoubleVec_t<DoubleType> &x, DoubleVec_t<DoubleType> &y) const
{
  dsAssert(compressed, "UNEXPECTED");

  const IntVec_t    &Cols = this->GetCols();
  const IntVec_t    &Rows = this->GetRows();
  const DoubleVec_t<DoubleType> &Vals = this->GetReal();
  if (compressionType_ == CompressionType::CCM)
  {
    RowScaleMultiply(Rows, Cols, Vals, x, y);
  }
  else if (compressionType_ == CompressionType::CRM)
  {
    ColScaleMultiply(Cols, Rows, Vals, x, y);
  }
}

template <typename DoubleType>
void CompressedMatrix<DoubleType>::Multiply(const ComplexDoubleVec_t<DoubleType> &x, ComplexDoubleVec_t<DoubleType> &y) const
{
  dsAssert(compressed, "UNEXPECTED");

  const IntVec_t    &Cols =  this->GetCols();
  const IntVec_t    &Rows =  this->GetRows();
  const ComplexDoubleVec_t<DoubleType> &Vals = this->GetComplex();
  if (compressionType_ == CompressionType::CCM)
  {
    ColScaleMultiply(Cols, Rows, Vals, x, y);
  }
  else if (compressionType_ == CompressionType::CRM)
  {
    RowScaleMultiply(Rows, Cols, Vals, x, y);
  }
}

template <typename DoubleType>
void CompressedMatrix<DoubleType>::TransposeMultiply(const ComplexDoubleVec_t<DoubleType> &x, ComplexDoubleVec_t<DoubleType> &y) const
{
  dsAssert(compressed, "UNEXPECTED");

  const IntVec_t    &Cols =  this->GetCols();
  const IntVec_t    &Rows =  this->GetRows();
  const ComplexDoubleVec_t<DoubleType> &Vals = this->GetComplex();
  if (compressionType_ == CompressionType::CCM)
  {
    RowScaleMultiply(Rows, Cols, Vals, x, y);
  }
  else if (compressionType_ == CompressionType::CRM)
  {
    ColScaleMultiply(Cols, Rows, Vals, x, y);
  }
}
}

template class dsMath::CompressedMatrix<double>;

#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class dsMath::CompressedMatrix<float128>;
#endif

