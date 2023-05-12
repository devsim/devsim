/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "Region.hh"
#ifndef OBJECT_CACHE_HH
#define OBJECT_CACHE_HH
#include <map>
#include <string>
template <typename T> class ObjectCache {
  public:
    bool GetEntry(const std::string &name, T &ent) const
    {
      bool ret = false;
      typename std::map<std::string, T>::const_iterator it = objectmap.find(name);
      if (it != objectmap.end())
      {
        ent = (*it).second;
        ret = true;
      }
      return ret;
    }

    void SetEntry(const std::string &name, const T &ent)
    {
      objectmap[name] = ent;
    }

    void clear()
    {
      objectmap.clear();
    }

  private:
    std::map<std::string, T> objectmap;
};
#endif
