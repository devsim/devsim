/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef DS_MATRIXENTRIES_HH
#define DS_MATRIXENTRIES_HH
#include "dsMathTypes.hh"
#include<vector>

namespace dsMath {

template <typename T> class RowColVal;

template <typename DoubleType>
using RealRowColVal = RowColVal<DoubleType>;

template <typename DoubleType>
using RealRowColValueVec = std::vector<RealRowColVal<DoubleType>>;

template <typename DoubleType>
using RHSEntry = std::pair<int, DoubleType>;

template <typename DoubleType>
using RHSEntryVec = std::vector<RHSEntry<DoubleType>>;

template <typename DoubleType>
using ComplexRowColVal = RowColVal<ComplexDouble_t<DoubleType>>;

template <typename DoubleType>
using ComplexRowColValueVec = std::vector<ComplexRowColVal<DoubleType>>;

template <typename T>
class RowColVal
{
   public:
      RowColVal(int r, int c, T v) : row(r), col(c), val(v) {}
      int row;
      int col;
      T   val;
};


}
#endif

