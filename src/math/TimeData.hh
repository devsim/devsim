/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef TIMEDATA_HH
#define TIMEDATA_HH
#include <vector>
#include <cstddef>

enum class TimePoint_t {TM0 = 0, TM1, TM2};

template <typename DoubleType>
class TimeData
{
    public:
        static TimeData &GetInstance();
        static void DestroyInstance();

        void SetI(TimePoint_t, const std::vector<DoubleType> &);
        void SetQ(TimePoint_t, const std::vector<DoubleType> &);

        //// from -> to
        void CopyI(TimePoint_t, TimePoint_t);
        void CopyQ(TimePoint_t, TimePoint_t);

        void ClearI(TimePoint_t);
        void ClearQ(TimePoint_t);

        void AssembleI(TimePoint_t, DoubleType, std::vector<DoubleType> &);
        void AssembleQ(TimePoint_t, DoubleType, std::vector<DoubleType> &);

        bool ExistsI(TimePoint_t tp)
        {
          return !IData[static_cast<size_t>(tp)].empty();
        }

        bool ExistsQ(TimePoint_t tp)
        {
          return !QData[static_cast<size_t>(tp)].empty();
        }

    private:

        TimeData();
        TimeData(TimeData &);
        TimeData &operator=(TimeData &);
        ~TimeData();
        static TimeData *instance;

        std::vector<std::vector<DoubleType > > IData;
        std::vector<std::vector<DoubleType > > QData;
};

#endif

