/***
DEVSIM
Copyright 2019 Devsim LLC

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

#include "GlobalData.hh"
#include "ObjectHolder.hh"
#include "OutputStream.hh"
#include <sstream>
#include <string>

namespace ThreadInfo {
size_t GetNumberOfThreads()
{
  size_t ret = 0;
  GlobalData &gdata = GlobalData::GetInstance();
  GlobalData::DBEntry_t dbent = gdata.GetDBEntryOnGlobal("threads_available");
  if (dbent.first)
  {
    ObjectHolder::IntegerEntry_t ient = dbent.second.GetInteger();
    if (!ient.first || ient.second < 0)
    {
      std::ostringstream os;
      os << "Expected valid positive number for \"threads_available\" parameter, but " << dbent.second.GetString() << " was given.\n";
      OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
      ret = 0;
    }
    else
    {
      ret = ient.second;
    }
  }

  return ret;
}

size_t GetMinimumTaskSize()
{
  size_t ret = 0;
  GlobalData &gdata = GlobalData::GetInstance();
  GlobalData::DBEntry_t dbent = gdata.GetDBEntryOnGlobal("threads_task_size");
  if (dbent.first)
  {
    ObjectHolder::IntegerEntry_t ient = dbent.second.GetInteger();
    if (!ient.first || ient.second < 0)
    {
      std::ostringstream os;
      os << "Expected valid positive number for \"threads_task_size\" parameter, but " << dbent.second.GetString() << " was given.\n";
      OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
      ret = 0;
    }
    else
    {
      ret = ient.second;
    }
  }

  return ret;
}
}

