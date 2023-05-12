/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "BlockPreconditioner.hh"
#include "SolverUtil.hh"
#include "dsAssert.hh"
#include "Matrix.hh"
#include "CompressedMatrix.hh"
#include "GlobalData.hh"
#include "Device.hh"
#include "Region.hh"
#include "MatrixEntries.hh"
#include "OutputStream.hh"

#include <algorithm>
#include <sstream>
#include <map>

#include <cmath>
using std::abs;

namespace dsMath {
template <typename T>
/// Sort first by increasing row
/// Then by decreasing value for each row
struct SpecialSort
{
   bool operator()(const RowColVal<T> &x, const RowColVal<T> &y) {
      bool ret = false;
      if (x.row < y.row)
      {
        ret = true;
      }
      else if (x.row == y.row)
      {
        ret = (abs(x.val) > abs(y.val));
      }
      return ret;
    }
};

template <typename DoubleType>
dsMath::CompressionType BlockPreconditioner<DoubleType>::GetRealMatrixCompressionType() const
{
  return dsMath::CompressionType::CCM;
}

template <typename DoubleType>
dsMath::CompressionType BlockPreconditioner<DoubleType>::GetComplexMatrixCompressionType() const
{
  return dsMath::CompressionType::CCM;
}

template <typename DoubleType>
BlockPreconditioner<DoubleType>::~BlockPreconditioner()
{
  delete block_matrix_;
  delete block_preconditioner_;
}

template <typename DoubleType>
BlockPreconditioner<DoubleType>::BlockPreconditioner(size_t numeqns, PEnum::TransposeType_t transpose) : Preconditioner<DoubleType>(numeqns, transpose), block_matrix_(nullptr), block_preconditioner_(nullptr), drop_tolerance_(0.5)
{
  block_preconditioner_ = CreateDirectPreconditioner<DoubleType>(numeqns);
}

template <typename DoubleType>
void BlockPreconditioner<DoubleType>::CreateBlockInfo()
{
  blockInfoList_.clear();

  GlobalData &gdata = GlobalData::GetInstance();
  const GlobalData::DeviceList_t &dlist = gdata.GetDeviceList();
  GlobalData::DeviceList_t::const_iterator dit  = dlist.begin();
  GlobalData::DeviceList_t::const_iterator dend = dlist.end();
  for ( ; dit != dend; ++dit)
  {
    const Device &device = *(dit->second);
    const Device::RegionList_t &rlist = device.GetRegionList();

    Device::RegionList_t::const_iterator rit  = rlist.begin();
    Device::RegionList_t::const_iterator rend = rlist.end();

    for ( ; rit != rend; ++rit)
    {
      const Region &region = *(rit->second);
      //// this calculation is based having all rows of the same equation together
      const size_t rmin = region.GetBaseEquationNumber();
      if (rmin != size_t(-1))
      {
        const size_t rmax = region.GetMaxEquationNumber();
        const size_t neqns = region.GetNumberEquations();
        const size_t nnodes = region.GetNumberNodes();

        for (size_t i = 0; i < neqns; ++i)
        {
          const size_t eqmin = rmin + i * nnodes;
          const size_t eqmax = eqmin + nnodes - 1;
          blockInfoList_.push_back(BlockInfo(eqmin, eqmax, rmin, rmax));
//          std::cerr << eqmin << " " << eqmax << " " << rmin << " " << rmax << "\n";
        }
      }
    }
  }

  std::sort(blockInfoList_.begin(), blockInfoList_.end());

  equationNumberToBlockMap_.clear();
  equationNumberToBlockMap_.resize(Preconditioner<DoubleType>::size(), size_t(-1));
  for (size_t i = 0; i < blockInfoList_.size(); ++i)
  {
    const BlockInfo &binfo = blockInfoList_[i];
    for (size_t j = binfo.min_eqnum_; j <= binfo.max_eqnum_; ++j)
    {
      equationNumberToBlockMap_[j] = i;
    }
  }
}

template <typename DoubleType> template <typename T>
void BlockPreconditioner<DoubleType>::ProcessBlockInfo(const IntVec_t &Cols, const IntVec_t &Rows, const std::vector<T> &Vals)
{
  const size_t cmax = Cols.size() - 1;

  //// first index into map is the block row
  //// second index is block column
  typedef std::vector<RowColVal<T> > rcvvec_t;
  typedef std::map<int, rcvvec_t>    colblock_t;
  typedef std::map<int, colblock_t>  rowcolblock_t;
  rowcolblock_t entries_per_block;

  size_t kept = 0;
  size_t outofrange = 0;
  size_t dropped = 0;
  for (size_t c = 0; c < cmax; ++c)
  {
    const size_t cbindex = equationNumberToBlockMap_[c];

    const size_t rimin = Cols[c];
    const size_t rimax = Cols[c+1];

    for (size_t ri = rimin; ri < rimax; ++ri)
    {
      const size_t r = Rows[ri];
      const T v = Vals[ri];

      const size_t rbindex = equationNumberToBlockMap_[r];
      if (rbindex == cbindex)
      {
#if 0
        if (r == c)
        {
          block_matrix_->AddEntry(r, c, v);
          ++kept;
        }
        else
        {
          entries_per_block[rbindex][cbindex].push_back(RowColVal<T>(r, c, v));
        }
#else
        entries_per_block[rbindex][cbindex].push_back(RowColVal<T>(r, c, v));
#endif
      }
      else
      {
        const BlockInfo &binfo = blockInfoList_[rbindex];
        if (!((c >= binfo.min_range_) && (c <= binfo.max_range_)))
        {
          //// These entries must always be added since tie interfaces, contacts
          block_matrix_->AddEntry(r, c, v);
          ++outofrange;
        }
        else
        {
          entries_per_block[rbindex][cbindex].push_back(RowColVal<T>(r, c, v));
        }
      }
    }
  }

  for (typename rowcolblock_t::iterator it = entries_per_block.begin(); it != entries_per_block.end(); ++it)
  {
    colblock_t &colblock = it->second;
    for (typename colblock_t::iterator jt = colblock.begin(); jt != colblock.end(); ++jt)
    {
      rcvvec_t &vec = jt->second;

      //// sorted by increasing row, then by decreasing value
      std::sort(vec.begin(), vec.end(), SpecialSort<T>());

      int    last_row = -1;
      DoubleType drop_tolerance = 0.0;
      bool   done_with_row = false;
      for (size_t i = 0; i < vec.size(); ++i)
      {
        RowColVal<T> &rcv = vec[i];

        const int &row = rcv.row;
        const int &col = rcv.col;
        T          val = rcv.val;
        const DoubleType val_mag = abs(val);

        if (row != last_row)
        {
          last_row       = row;
          drop_tolerance = drop_tolerance_ * val_mag;
          done_with_row  = false;
        }

        if (done_with_row)
        {
          ++dropped;
        }
        else if ((val_mag < drop_tolerance) && (row != col))
        {
          done_with_row = true;
          ++dropped;
        }
        else
        {
          block_matrix_->AddEntry(row, col, val);
          ++kept;
        }
      }

#if 0
      size_t len = vec.size()/200+1;
      for (size_t i = 0; i < len; ++i)
      {
        RowColVal<T> &rcv = vec[i];
        block_matrix_->AddEntry(rcv.row, rcv.col, rcv.val);
      }
      reclaimed += len;
      dropped += (vec.size() - len);
      if (len > 1)
      {
        std::cerr << "rel: " << it->first << " " << jt->first << " " <<  vec[len-1].val/vec[0].val << "\n";
      }
#endif
    }
  }
  {
    DoubleType drop_percent = 100.0 * static_cast<DoubleType>(dropped) / (dropped+kept+outofrange);
    std::ostringstream os;
    os
        << "drop tolerance_ " << drop_tolerance_
        << " dropped " << dropped
        << " kept " << kept
        << " outofrange " << outofrange
        << " drop % " << drop_percent <<
        "\n";
    OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
  }
}

template <typename DoubleType>
void BlockPreconditioner<DoubleType>::CreateBlockMatrix(CompressedMatrix<DoubleType> *cm)
{
//  block_matrix_->ClearMatrix();

  dsAssert(cm->GetCompressionType() == CompressionType::CCM, "UNEXPECTED");

  if (cm->GetMatrixType() == MatrixType::REAL)
  {
    const IntVec_t    &Cols = cm->GetCols();
    const IntVec_t    &Rows = cm->GetRows();
    const DoubleVec_t<DoubleType> &Vals = cm->GetReal();
    ProcessBlockInfo(Cols, Rows, Vals);
  }
  else if (cm->GetMatrixType() == MatrixType::COMPLEX)
  {
    const IntVec_t    &Cols = cm->GetCols();
    const IntVec_t    &Rows = cm->GetRows();
    const ComplexDoubleVec_t<DoubleType> &Vals = cm->GetComplex();
    ProcessBlockInfo(Cols, Rows, Vals);
  }
}

template <typename DoubleType>
bool BlockPreconditioner<DoubleType>::DerivedLUFactor(Matrix<DoubleType> *m)
{
  CompressedMatrix<DoubleType> *cm = dynamic_cast<CompressedMatrix<DoubleType> *>(m);
  if (!cm)
  {
    dsAssert(cm != nullptr, "UNEXPECTED");
    return false;
  }
  dsAssert(cm->GetCompressionType() == CompressionType::CCM, "UNEXPECTED");

  if (!block_matrix_)
  {
    block_matrix_ = new CompressedMatrix<DoubleType>(cm->size(), cm->GetMatrixType(), cm->GetCompressionType());
    CreateBlockInfo();
  }
  else
  {
    dsAssert(cm->size() == block_matrix_->size(), "UNEXPECTED");
    dsAssert(cm->GetMatrixType() == block_matrix_->GetMatrixType(), "UNEXPECTED");
    dsAssert(cm->GetCompressionType() == block_matrix_->GetCompressionType(), "UNEXPECTED");
    block_matrix_->ClearMatrix();
  }

  CreateBlockMatrix(cm);
  block_matrix_->Finalize();


  return block_preconditioner_->LUFactor(block_matrix_);
}

template <typename DoubleType>
void BlockPreconditioner<DoubleType>::DerivedLUSolve(DoubleVec_t<DoubleType> &x, const DoubleVec_t<DoubleType> &b) const
{
  block_preconditioner_->LUSolve(x, b);
}

template <typename DoubleType>
void BlockPreconditioner<DoubleType>::DerivedLUSolve(ComplexDoubleVec_t<DoubleType> &x, const ComplexDoubleVec_t<DoubleType> &b) const
{
  block_preconditioner_->LUSolve(x, b);
}
}

template class dsMath::BlockPreconditioner<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class dsMath::BlockPreconditioner<float128>;
#endif

