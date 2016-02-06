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

TimeData *TimeData::instance = 0;

TimeData::TimeData() : IData(3), QData(3)
{
}

TimeData &TimeData::GetInstance()
{
    if (!instance)
    {
        instance = new TimeData;
    }
    return *instance;
}

TimeData::~TimeData()
{
}

void TimeData::DestroyInstance()
{
    if (instance)
    {
        delete instance;
    }
    instance = 0;
}

//// TODO: transfer ownership
void TimeData::SetI(TimePoint_t tp, const std::vector<double> &v)
{
  IData[tp] = v;
}

void TimeData::SetQ(TimePoint_t tp, const std::vector<double> &v)
{
  QData[tp] = v;
}

void TimeData::CopyI(TimePoint_t fr, TimePoint_t to)
{
  IData[to] = IData[fr];
}

void TimeData::CopyQ(TimePoint_t fr, TimePoint_t to)
{
  QData[to] = QData[fr];
}

void TimeData::ClearI(TimePoint_t tp)
{
  IData[tp].clear();
}

void TimeData::ClearQ(TimePoint_t tp)
{
  QData[tp].clear();
}

void TimeData::AssembleI(TimePoint_t tp, double scale, std::vector<double> &v)
{
  dsAssert(!IData[tp].empty(),            "UNEXPECTED missing time data");
  dsAssert( IData[tp].size() == v.size(), "UNEXPECTED time data mismatch");

  const std::vector<double> &data = IData[tp];

  for (size_t i = 0; i < v.size(); ++i)
  {
    v[i] += scale * data[i];
  }
}

void TimeData::AssembleQ(TimePoint_t tp, double scale, std::vector<double> &v)
{
  dsAssert(!QData[tp].empty(),            "UNEXPECTED missing time data");
  dsAssert( QData[tp].size() == v.size(), "UNEXPECTED time data mismatch");

  const std::vector<double> &data = QData[tp];

  for (size_t i = 0; i < v.size(); ++i)
  {
    v[i] += scale * data[i];
  }
}

