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

#ifndef TIMEDATA_HH
#define TIMEDATA_HH
#include <vector>

class TimeData
{
    public:
        static TimeData &GetInstance();
        static void DestroyInstance();

        enum TimePoint_t {TM0 = 0, TM1, TM2};

        void SetI(TimePoint_t, const std::vector<double> &);
        void SetQ(TimePoint_t, const std::vector<double> &);

        //// from -> to
        void CopyI(TimePoint_t, TimePoint_t);
        void CopyQ(TimePoint_t, TimePoint_t);

        void ClearI(TimePoint_t);
        void ClearQ(TimePoint_t);

        void AssembleI(TimePoint_t, double, std::vector<double> &);
        void AssembleQ(TimePoint_t, double, std::vector<double> &);

        bool ExistsI(TimePoint_t tp)
        {
          return !IData[tp].empty();
        }

        bool ExistsQ(TimePoint_t tp)
        {
          return !QData[tp].empty();
        }

    private:

        TimeData();
        TimeData(TimeData &);
        TimeData &operator=(TimeData &);
        ~TimeData();
        static TimeData *instance;

        std::vector<std::vector<double > > IData;
        std::vector<std::vector<double > > QData;
};

#endif

