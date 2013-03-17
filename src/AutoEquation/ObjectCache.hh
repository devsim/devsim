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
