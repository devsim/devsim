/***
DEVSIM
Copyright 2013 DEVSIM LLC

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

#ifndef DS_MATRIXENTRIES_HH
#define DS_MATRIXENTRIES_HH
#include<vector>
#include<complex>

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
using ComplexRowColVal = RowColVal<std::complex<DoubleType>>;

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

