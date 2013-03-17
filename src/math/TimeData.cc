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

