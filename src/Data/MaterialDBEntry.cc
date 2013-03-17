/***
DEVSIM
Copyright 2013 Devsim LLC

This file is part of DEVSIM.

DEVSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, version 3.

DEVSIM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with DEVSIM.  If not, see <http://www.gnu.org/licenses/>.
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

MaterialDBEntry::MaterialDBEntry() : entry_type_(UNDEFINED)
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




