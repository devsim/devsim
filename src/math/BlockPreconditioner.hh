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

#ifndef BLOCK_PRECONDITIONER_HH
#define BLOCK_PRECONDITIONER_HH
#include "Preconditioner.hh"
#include "dsAssert.hh"
#include <vector>

namespace dsMath {
class Matrix;
class CompressedMatrix;

struct BlockInfo
{
  BlockInfo(size_t emin, size_t emax, size_t rmin, size_t rmax) : min_eqnum_(emin), max_eqnum_(emax), min_range_(rmin), max_range_(rmax)
  {
  }

  bool operator<(const BlockInfo &block) const
  {
    bool ret = false;
    if (min_eqnum_ < block.min_eqnum_)
    {
      ret = true;
    }
    else if (min_eqnum_ == block.min_eqnum_)
    {
      dsAssert(
        (max_eqnum_ == block.max_eqnum_) &&
        (min_range_ == block.min_range_) &&
        (max_range_ == block.max_range_)
      , "UNEXPECTED");
    }
    return ret;
  }

  bool operator==(const BlockInfo &block) const
  {
    return ((min_eqnum_ == block.min_eqnum_) &&
        (max_eqnum_ == block.max_eqnum_) &&
        (min_range_ == block.min_range_) &&
        (max_range_ == block.max_range_));
  }

  /// Assume that block diagonal so min/max rows share same range as min/max columns
  size_t min_eqnum_;
  size_t max_eqnum_;
  /// Assume that we break this up into 
  size_t min_range_;
  size_t max_range_;
};

class BlockPreconditioner : public Preconditioner {
  public:
    virtual ~BlockPreconditioner();

    BlockPreconditioner(size_t /*numeqns*/, Preconditioner::TransposeType_t /*tranpose*/);

  protected:
    void DerivedLUSolve(DoubleVec_t &x, const DoubleVec_t &b) const;
    void DerivedLUSolve(ComplexDoubleVec_t &x, const ComplexDoubleVec_t &b) const;
    bool DerivedLUFactor(Matrix *);     // Factor the matrix

  private:
    void CreateBlockInfo();
    void CreateBlockMatrix(CompressedMatrix *);

    template <typename T>
    void ProcessBlockInfo(const IntVec_t &, const IntVec_t &, const std::vector<T> &);

    typedef std::vector<BlockInfo> BlockInfoList_t;
    //// index into block info list for each equation number
    typedef std::vector<size_t>    EquationNumberToBlockMap_t;

    BlockInfoList_t            blockInfoList_;
    EquationNumberToBlockMap_t equationNumberToBlockMap_;

    CompressedMatrix *block_matrix_;
    Preconditioner   *block_preconditioner_;
    double            drop_tolerance_;
};

}
#endif
