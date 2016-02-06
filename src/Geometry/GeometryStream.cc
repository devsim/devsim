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

#include "GeometryStream.hh"
#include "GlobalData.hh"
#include "ObjectHolder.hh"
#include "Device.hh"
#include "Region.hh"
#include "Contact.hh"
#include "Interface.hh"

void GeometryStream::WriteOut(OutputStream::OutputType ot, const Device &device, const std::string &msg)
{
  const GlobalData &ginst = GlobalData::GetInstance();
  GlobalData::DBEntry_t dbent = ginst.GetDBEntryOnDevice(device.GetName(), "debug_level");
  if (!dbent.first)
  {
    OutputStream::WriteOut(ot, OutputStream::V0, msg);
  }
  else
  {
    OutputStream::WriteOut(ot, OutputStream::GetVerbosity(dbent.second.GetString()), msg);
  }
}

void GeometryStream::WriteOut(OutputStream::OutputType ot, const Contact &contact, const std::string &msg)
{
  const GlobalData &ginst = GlobalData::GetInstance();
  GlobalData::DBEntry_t dbent = ginst.GetDBEntryOnRegion(contact.GetRegion(), "debug_level");
  if (!dbent.first)
  {
    OutputStream::WriteOut(ot, OutputStream::V0, msg);
  }
  else
  {
    OutputStream::WriteOut(ot, OutputStream::GetVerbosity(dbent.second.GetString()), msg);
  }
}

void GeometryStream::WriteOut(OutputStream::OutputType ot, const Interface &interface, const std::string &msg)
{
  const GlobalData &ginst = GlobalData::GetInstance();

  GlobalData::DBEntry_t dbent0 = ginst.GetDBEntryOnRegion(interface.GetRegion0(), "debug_level");
  GlobalData::DBEntry_t dbent1 = ginst.GetDBEntryOnRegion(interface.GetRegion1(), "debug_level");

  if (!(dbent0.first || dbent1.first))
  {
    //// no entries
    OutputStream::WriteOut(ot, OutputStream::V0, msg);
  }
  else if (dbent0.first && dbent1.first)
  {
    OutputStream::Verbosity_t verb0 = OutputStream::GetVerbosity(dbent0.second.GetString());
    OutputStream::Verbosity_t verb1 = OutputStream::GetVerbosity(dbent1.second.GetString());
    if (verb0 > verb1)
    {
      OutputStream::WriteOut(ot, verb0, msg);
    }
    else
    {
      OutputStream::WriteOut(ot, verb1, msg);
    }
  }
  else if (dbent0.first)
  {
    OutputStream::WriteOut(ot, OutputStream::GetVerbosity(dbent0.second.GetString()), msg);
  }
  else if (dbent1.first)
  {
    OutputStream::WriteOut(ot, OutputStream::GetVerbosity(dbent1.second.GetString()), msg);
  }
}

void GeometryStream::WriteOut(OutputStream::OutputType ot, const Region &region, const std::string &msg)
{
  const GlobalData &ginst = GlobalData::GetInstance();
  GlobalData::DBEntry_t dbent = ginst.GetDBEntryOnRegion(&region, "debug_level");
  if (!dbent.first)
  {
    OutputStream::WriteOut(ot, OutputStream::V0, msg);
  }
  else
  {
    OutputStream::WriteOut(ot, OutputStream::GetVerbosity(dbent.second.GetString()), msg);
  }
}


