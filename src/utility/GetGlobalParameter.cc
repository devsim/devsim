/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "GetGlobalParameter.hh"
#include "GlobalData.hh"
#include "ObjectHolder.hh"
std::string GetGlobalParameterStringOptional(const std::string &name)
{
  std::string ret;
  const GlobalData &ginst = GlobalData::GetInstance();
  GlobalData::DBEntry_t dbent = ginst.GetDBEntryOnGlobal(name);
  if (dbent.first)
  {
    ret = dbent.second.GetString();
  }
  return ret;
}

