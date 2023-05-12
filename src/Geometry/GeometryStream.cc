/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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
    OutputStream::WriteOut(ot, OutputStream::Verbosity_t::V0, msg);
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
    OutputStream::WriteOut(ot, OutputStream::Verbosity_t::V0, msg);
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
    OutputStream::WriteOut(ot, OutputStream::Verbosity_t::V0, msg);
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
    OutputStream::WriteOut(ot, OutputStream::Verbosity_t::V0, msg);
  }
  else
  {
    OutputStream::WriteOut(ot, OutputStream::GetVerbosity(dbent.second.GetString()), msg);
  }
}


