/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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




