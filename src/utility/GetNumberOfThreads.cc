/***
DEVSIM
Copyright 2019 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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

