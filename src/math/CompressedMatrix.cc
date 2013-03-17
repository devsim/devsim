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

#include "CompressedMatrix.hh"
#include "MatrixEntries.hh"
#include "OutputStream.hh"
#include "dsAssert.hh"

#include <sstream>
#include <utility>
#include <algorithm>

namespace dsMath {
CompressedMatrix::CompressedMatrix(size_t sz, MatrixType mt, CompressionType ct) : Matrix(sz), matType_(mt), compressionType_(ct), compressed(false)
{
  Symbolic_.resize(size());
  OutOfBandEntries_Real.resize(size());
  if (GetMatrixType() == COMPLEX)
  {
    OutOfBandEntries_Imag.resize(size());
  }
}

CompressedMatrix::~CompressedMatrix()
{
}

void CompressedMatrix::AddSymbolic(int r, int c)
{
  if (compressionType_ == CRM)
  {
    int tmp = r;
    r = c;
    c = tmp;
  }

  /// remember compressed column
  /// need to make it possible to add out of band entries
#ifndef NDEBUG
  dsAssert(!compressed, "UNEXPECTED");
#endif
  Symbolic_[c].insert(std::make_pair(r, 0)); // should create implicit entry
}

/// Create Matrix from symbolic info
void CompressedMatrix::CreateMatrix()
{
  const size_t cols = Symbolic_.size();
#ifndef NDEBUG
  dsAssert(cols == size(), "UNEXPECTED");
#endif

  Ap_.resize(size()+1);
  Ai_.clear();
  Ai_.reserve(size());

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
  Ap_[size()] = r;
  
  // Ye old swap trick
  IntVec_t(Ai_).swap(Ai_);
  // reserve room
  Ax_.clear();
  Ax_.resize(Ai_.size());
  if (GetMatrixType() == COMPLEX) {
    Az_.clear();
    Az_.resize(Ai_.size());
  }

  compressed = true;
}

void CompressedMatrix::SetCompressed(bool x)
{
    compressed = true;
}

bool CompressedMatrix::GetCompressed()
{
    return compressed;
}

const dsMath::IntVec_t &CompressedMatrix::GetCols() const
{
  dsAssert(compressed, "UNEXPECTED");

  if (compressionType_ == CCM)
  {
    return Ap_;
  }
  else if (compressionType_ == CRM)
  {
    return Ai_;
  }
  else
  {
    dsAssert(0, "UNEXPECTED");
  }

  return Ap_;
}

const dsMath::IntVec_t &CompressedMatrix::GetRows() const
{
  dsAssert(compressed, "UNEXPECTED");

  if (compressionType_ == CCM)
  {
    return Ai_;
  }
  else if (compressionType_ == CRM)
  {
    return Ap_;
  }
  else
  {
    dsAssert(0, "UNEXPECTED");
  }
  return Ai_;
}

const dsMath::DoubleVec_t &CompressedMatrix::GetReal() const
{
  dsAssert(compressed, "UNEXPECTED");

  return Ax_;
}

const dsMath::DoubleVec_t &CompressedMatrix::GetImag() const
{
  dsAssert(compressed, "UNEXPECTED");

  return Az_;
}

const dsMath::ComplexDoubleVec_t CompressedMatrix::GetComplex() const
{
  dsAssert(compressed, "UNEXPECTED");
  dsAssert(Ax_.size() == Az_.size(), "UNEXPECTED");

  size_t len = Ax_.size();

  ComplexDoubleVec_t ret(len);

  /// Not the most efficient
  for (size_t i = 0; i < len; ++i)
  {
    ret[i] = ComplexDouble_t(Ax_[i], Az_[i]);
  }

  return ret;
}

void CompressedMatrix::AddEntry(int r, int c, double v)
{
#ifndef NDEBUG
  dsAssert(static_cast<size_t>(r) < size(), "UNEXPECTED");
  dsAssert(static_cast<size_t>(c) < size(), "UNEXPECTED");
#endif

  if (v == 0.0)
  {
    return;
  }
  else if (compressionType_ == CRM)
  {
    int tmp = r;
    r = c;
    c = tmp;
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
      AddSymbolic(r, c);
      OutOfBandEntries_Real[r][c] += v;
    }
  }
  else
  {
    AddSymbolic(r, c);
    /// the double entry is initialized to zero (property of map)
    OutOfBandEntries_Real[r][c] += v;
  }
}

/// Model this after AddEntry
void CompressedMatrix::AddImagEntry(int r, int c, double v)
{
#ifndef NDEBUG
  dsAssert(GetMatrixType() == COMPLEX, "UNEXPECTED");
  dsAssert(static_cast<size_t>(r) < size(), "UNEXPECTED");
  dsAssert(static_cast<size_t>(c) < size(), "UNEXPECTED");
#endif

  if (v == 0.0)
  {
    return;
  }
  else if (compressionType_ == CRM)
  {
    int tmp = r;
    r = c;
    c = tmp;
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
      AddSymbolic(r, c);
      OutOfBandEntries_Imag[r][c] += v;
    }
  }
  else
  {
    AddSymbolic(r, c);
    /// the double entry is initialized to zero (property of map)
    OutOfBandEntries_Imag[r][c] += v;
  }
}

void CompressedMatrix::AddEntry(int r, int c, ComplexDouble_t v)
{
  if (compressionType_ == CRM)
  {
    int tmp = r;
    r = c;
    c = tmp;
  }

  const double rv = v.real();
  const double iv = v.imag();

  if (rv != 0.0)
  {
    AddEntry(r, c, rv);
  }

  if (iv != 0.0)
  {
    AddImagEntry(r, c, iv);
  }
}

void CompressedMatrix::DecompressMatrix()
{
  if (!compressed)
  {
    return;
  }
  std::ostringstream os; 
  os << "Matrix Decompress!!! Symbolic pattern changed\n";
  OutputStream::WriteOut(OutputStream::VERBOSE1, os.str());
  compressed = false;
  size_t sz = Ap_.size() - 1;

#ifndef NDEBUG
  dsAssert(sz == size(), "UNEXPECTED");
#endif

  /// size is 1 past in compressed column
  for (size_t i = 0; i < sz; ++i)
  {
    const size_t beg = Ap_[i];
    const size_t end = Ap_[i+1];
    for (size_t j = beg; j < end; ++ j)
    {
      AddEntry(Ai_[j], i, Ax_[j]);
    }
    if (GetMatrixType() == COMPLEX)
    {
      for (size_t j = beg; j < end; ++ j)
      {
        const double z = Az_[j];
        if (z != 0.0)
        {
          AddEntry(Ai_[j], i, z);
        }
      }
    }
  }
  Ai_.clear();
  Ax_.clear();
  Az_.clear();
}


void CompressedMatrix::Finalize()
{
  if (!compressed)
  {
    symbolicstatus_ = NEW_SYMBOLIC;

    CreateMatrix();
    for (size_t i = 0; i < OutOfBandEntries_Real.size(); ++i)
    {
      ColValueEntry::iterator it = OutOfBandEntries_Real[i].begin();
      const ColValueEntry::iterator itend = OutOfBandEntries_Real[i].end();
      for ( ; it != itend; ++it)
      {
        AddEntry(i, it->first, it->second);
      }
    }
    OutOfBandEntries_Real.clear();
    OutOfBandEntries_Real.resize(size());

    if (GetMatrixType() == COMPLEX)
    {
      for (size_t i = 0; i < OutOfBandEntries_Imag.size(); ++i)
      {
        ColValueEntry::iterator it = OutOfBandEntries_Imag[i].begin();
        const ColValueEntry::iterator itend = OutOfBandEntries_Imag[i].end();
        for ( ; it != itend; ++it)
        {
          AddImagEntry(i, it->first, it->second);
        }
      }
      OutOfBandEntries_Imag.clear();
      OutOfBandEntries_Imag.resize(size());
    }
  }
  else
  {
    symbolicstatus_ = SAME_SYMBOLIC;
  }
}

void CompressedMatrix::ClearMatrix()
{
#ifndef NDEBUG
  dsAssert(!Ax_.empty(), "UNEXPECTED");
#endif
//  compressed = false;
  size_t sz = Ax_.size();
  Ax_.clear();
  Ax_.resize(sz);
  if (GetMatrixType() == COMPLEX)
  {
    Az_.clear();
    Az_.resize(sz);
  }

  OutOfBandEntries_Real.clear();
  OutOfBandEntries_Real.resize(sz);
  if (GetMatrixType() == COMPLEX)
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

void CompressedMatrix::Multiply(const DoubleVec_t &x, DoubleVec_t &y) const
{
  dsAssert(compressed, "UNEXPECTED");

  const IntVec_t    &Cols = this->GetCols();
  const IntVec_t    &Rows = this->GetRows();
  const DoubleVec_t &Vals = this->GetReal();
  if (compressionType_ == CCM)
  {
    ColScaleMultiply(Cols, Rows, Vals, x, y);
  }
  else if (compressionType_ == CRM)
  {
    RowScaleMultiply(Rows, Cols, Vals, x, y);
  }
}

void CompressedMatrix::TransposeMultiply(const DoubleVec_t &x, DoubleVec_t &y) const
{
  dsAssert(compressed, "UNEXPECTED");

  const IntVec_t    &Cols = this->GetCols();
  const IntVec_t    &Rows = this->GetRows();
  const DoubleVec_t &Vals = this->GetReal();
  if (compressionType_ == CCM)
  {
    RowScaleMultiply(Rows, Cols, Vals, x, y);
  }
  else if (compressionType_ == CRM)
  {
    ColScaleMultiply(Cols, Rows, Vals, x, y);
  }
}

void CompressedMatrix::Multiply(const ComplexDoubleVec_t &x, ComplexDoubleVec_t &y) const
{
  dsAssert(compressed, "UNEXPECTED");

  const IntVec_t    &Cols =  this->GetCols();
  const IntVec_t    &Rows =  this->GetRows();
  const ComplexDoubleVec_t &Vals = this->GetComplex();
  if (compressionType_ == CCM)
  {
    ColScaleMultiply(Cols, Rows, Vals, x, y);
  }
  else if (compressionType_ == CRM)
  {
    RowScaleMultiply(Rows, Cols, Vals, x, y);
  }
}

void CompressedMatrix::TransposeMultiply(const ComplexDoubleVec_t &x, ComplexDoubleVec_t &y) const
{
  dsAssert(compressed, "UNEXPECTED");

  const IntVec_t    &Cols =  this->GetCols();
  const IntVec_t    &Rows =  this->GetRows();
  const ComplexDoubleVec_t &Vals = this->GetComplex();
  if (compressionType_ == CCM)
  {
    RowScaleMultiply(Rows, Cols, Vals, x, y);
  }
  else if (compressionType_ == CRM)
  {
    ColScaleMultiply(Cols, Rows, Vals, x, y);
  }
}
}

