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

#include "GetGlobalParameter.hh"
#include "GlobalData.hh"
#include "ObjectHolder.hh"
std::string GetGlobalParameterStringOptional(const std::string &name) {
  std::string ret;
  const GlobalData &ginst = GlobalData::GetInstance();
  GlobalData::DBEntry_t dbent = ginst.GetDBEntryOnGlobal(name);
  if (dbent.first) {
    ret = dbent.second.GetString();
  }
  return ret;
}

void *GetMainInterpreter() {
  return GlobalData::GetInstance().GetInterpreter();
}

void SetMainInterpreter(void *interp) {
  GlobalData::GetInstance().SetInterpreter(interp);
}
