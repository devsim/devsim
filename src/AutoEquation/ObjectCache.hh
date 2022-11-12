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
