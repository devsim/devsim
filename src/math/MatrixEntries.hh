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

#ifndef DS_MATRIXENTRIES_HH
#define DS_MATRIXENTRIES_HH
#include<vector>
#include<functional>
#include<complex>

namespace dsMath {

template <typename T> class RowColVal;
typedef RowColVal<double> RealRowColVal;
typedef std::vector<RealRowColVal > RealRowColValueVec;
typedef RowColVal<std::complex<double> > ComplexRowColVal;
typedef std::vector<ComplexRowColVal > ComplexRowColValueVec;
typedef std::pair<int, double> RHSEntry;
typedef std::vector<RHSEntry>  RHSEntryVec;

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
