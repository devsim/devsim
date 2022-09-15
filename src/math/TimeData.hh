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

#ifndef TIMEDATA_HH
#define TIMEDATA_HH
#include <cstddef>
#include <vector>

enum class TimePoint_t { TM0 = 0, TM1, TM2 };

template <typename DoubleType> class TimeData {
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

  bool ExistsI(TimePoint_t tp) {
    return !IData[static_cast<size_t>(tp)].empty();
  }

  bool ExistsQ(TimePoint_t tp) {
    return !QData[static_cast<size_t>(tp)].empty();
  }

private:
  TimeData();
  TimeData(TimeData &);
  TimeData &operator=(TimeData &);
  ~TimeData();
  static TimeData *instance;

  std::vector<std::vector<DoubleType>> IData;
  std::vector<std::vector<DoubleType>> QData;
};

#endif
