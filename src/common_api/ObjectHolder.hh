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

#ifndef OBJHOLDER_HH
#define OBJHOLDER_HH
#include <string>
#include <utility>
#include <vector>
#include <map>


class ObjectHolder;
/*Used to create Dictionaries*/
typedef std::map<std::string, ObjectHolder> ObjectHolderMap_t;
typedef std::vector<ObjectHolder> ObjectHolderList_t;

class ObjectHolder {
  public:
    ObjectHolder();
    ObjectHolder(const ObjectHolder &);
    ObjectHolder &operator=(const ObjectHolder &);
    ~ObjectHolder();

    explicit ObjectHolder(void *);

    explicit ObjectHolder(const std::string &);

    explicit ObjectHolder(double);
    explicit ObjectHolder(int);
    //// Guaranteed these do not change passed values
    explicit ObjectHolder(ObjectHolderMap_t &);
    explicit ObjectHolder(ObjectHolderList_t &);



    const void *GetObject() const;
    void *GetObject();

    //// The belief is that this cannot fail
    std::string GetString() const;

    typedef std::pair<bool, double> DoubleEntry_t;
    typedef std::pair<bool, bool>   BooleanEntry_t;
    typedef std::pair<bool, int>    IntegerEntry_t;
    DoubleEntry_t  GetDouble() const;
    BooleanEntry_t GetBoolean() const;
    IntegerEntry_t GetInteger() const;
    bool           IsList() const;
    bool           GetDoubleList(std::vector<double> &) const;

    bool empty()
    {
      return !object_;
    }

    void clear();

    bool GetListOfObjects(ObjectHolderList_t &) const;
    bool GetHashKeys(ObjectHolderList_t &) const;
    bool GetHashMap(ObjectHolderMap_t &) const;

  private:
    void *object_;
    
};
#endif

