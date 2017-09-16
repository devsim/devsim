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

#include "TimeData.hh"
#include "dsAssert.hh"

template <>
TimeData<double> *TimeData<double>::instance = 0;

#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template <>
TimeData<float128> *TimeData<float128>::instance = 0;
#endif

template <typename DoubleType>
TimeData<DoubleType>::TimeData() : IData(3), QData(3)
{
}

template <typename DoubleType>
TimeData<DoubleType> &TimeData<DoubleType>::GetInstance()
{
    if (!instance)
    {
        instance = new TimeData<DoubleType>;
    }
    return *instance;
}

template <typename DoubleType>
TimeData<DoubleType>::~TimeData()
{
}

template <typename DoubleType>
void TimeData<DoubleType>::DestroyInstance()
{
    if (instance)
    {
        delete instance;
    }
    instance = 0;
}

template <typename DoubleType>
void TimeData<DoubleType>::SetI(TimePoint_t tp, const std::vector<DoubleType> &v)
{
  IData[static_cast<size_t>(tp)] = v;
}

template <typename DoubleType>
void TimeData<DoubleType>::SetQ(TimePoint_t tp, const std::vector<DoubleType> &v)
{
  QData[static_cast<size_t>(tp)] = v;
}

template <typename DoubleType>
void TimeData<DoubleType>::CopyI(TimePoint_t fr, TimePoint_t to)
{
  IData[static_cast<size_t>(to)] = IData[static_cast<size_t>(fr)];
}

template <typename DoubleType>
void TimeData<DoubleType>::CopyQ(TimePoint_t fr, TimePoint_t to)
{
  QData[static_cast<size_t>(to)] = QData[static_cast<size_t>(fr)];
}

template <typename DoubleType>
void TimeData<DoubleType>::ClearI(TimePoint_t tp)
{
  IData[static_cast<size_t>(tp)].clear();
}

template <typename DoubleType>
void TimeData<DoubleType>::ClearQ(TimePoint_t tp)
{
  QData[static_cast<size_t>(tp)].clear();
}

template <typename DoubleType>
void TimeData<DoubleType>::AssembleI(TimePoint_t tp, DoubleType scale, std::vector<DoubleType> &v)
{
  dsAssert(!IData[static_cast<size_t>(tp)].empty(),            "UNEXPECTED missing time data");
  dsAssert( IData[static_cast<size_t>(tp)].size() == v.size(), "UNEXPECTED time data mismatch");

  const std::vector<DoubleType> &data = IData[static_cast<size_t>(tp)];

  for (size_t i = 0; i < v.size(); ++i)
  {
    v[i] += scale * data[i];
  }
}

template <typename DoubleType>
void TimeData<DoubleType>::AssembleQ(TimePoint_t tp, DoubleType scale, std::vector<DoubleType> &v)
{
  dsAssert(!QData[static_cast<size_t>(tp)].empty(),            "UNEXPECTED missing time data");
  dsAssert( QData[static_cast<size_t>(tp)].size() == v.size(), "UNEXPECTED time data mismatch");

  const std::vector<DoubleType> &data = QData[static_cast<size_t>(tp)];

  for (size_t i = 0; i < v.size(); ++i)
  {
    v[i] += scale * data[i];
  }
}

template class TimeData<double>;

#ifdef DEVSIM_EXTENDED_PRECISION
template class TimeData<float128>;
#endif


