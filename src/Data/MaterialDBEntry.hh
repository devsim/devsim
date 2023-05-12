/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef MATERIALDBENTRY_HH
#define MATERIALDBENTRY_HH
#include "ObjectHolder.hh"

class MaterialDBEntry {
  public:
    enum class EntryType_t {UNDEFINED, FROMDB, MODIFIED};

    MaterialDBEntry(const std::string &, const std::string &, const ObjectHolder &, EntryType_t);
//    MaterialDBEntry(const std::string &, const std::string &, const std::string &, const ObjectHolder &, EntryType_t);

#if 0
    const std::string &GetParameter() {
      return parameter_name_;
    }
#endif

    EntryType_t GetEntryType() const {
      return entry_type_;
    }

    const std::string &GetUnit() const {
      return unit_;
    }

    const std::string &GetDescription() const {
      return description_;
    }

    const ObjectHolder &GetValue()  const {
      return value_;
    }

    ObjectHolder &GetValue()  {
      return value_;
    }

    void SetValue(ObjectHolder t) {
      value_ = t;
    }

    void SetType(EntryType_t t) {
      entry_type_ = t;
    }

    MaterialDBEntry();

    MaterialDBEntry(const MaterialDBEntry &);

    MaterialDBEntry &operator=(const MaterialDBEntry &);

  private:

//    std::string   parameter_name_;
    std::string   unit_;
    std::string   description_;
    ObjectHolder  value_;
    EntryType_t   entry_type_;
};

#endif

