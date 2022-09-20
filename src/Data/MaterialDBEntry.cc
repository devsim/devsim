/***
DEVSIM
Copyright 2013 DEVSIM LLC

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

#include "MaterialDBEntry.hh"

#if 0
MaterialDBEntry::MaterialDBEntry(const std::string &p, const std::string &u, const std::string &d, const ObjectHolder &v, EntryType_t et) :
  parameter_name_(p),
  unit_(u),
  description_(d),
  value_(v),
  entry_type_(et)
{
}
#endif

MaterialDBEntry::MaterialDBEntry() : entry_type_(EntryType_t::UNDEFINED)
{
}

MaterialDBEntry::MaterialDBEntry(const MaterialDBEntry &o) :
  unit_(o.unit_),
  description_(o.description_),
  value_(o.value_),
  entry_type_(o.entry_type_)
{
}

MaterialDBEntry &MaterialDBEntry::operator=(const MaterialDBEntry &o)
{
  if (this != &o)
  {
    unit_ = o.unit_;
    description_ = o.description_;
    value_ = o.value_;
    entry_type_ = o.entry_type_;
  }
  return *this;
}

MaterialDBEntry::MaterialDBEntry(const std::string &u, const std::string &d, const ObjectHolder &v, EntryType_t et) :
  unit_(u),
  description_(d),
  value_(v),
  entry_type_(et)
{
}




