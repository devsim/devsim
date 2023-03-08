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

#ifndef OBJHOLDER_HH
#define OBJHOLDER_HH
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <cstddef>


class ObjectHolder;
/*Used to create Dictionaries*/
typedef std::map<std::string, ObjectHolder> ObjectHolderMap_t;
typedef std::vector<ObjectHolder> ObjectHolderList_t;

class ObjectHolder {
  public:
    ObjectHolder();
    ObjectHolder(const ObjectHolder &);
    ObjectHolder &operator=(const ObjectHolder &);
    ObjectHolder(ObjectHolder &&);
    ObjectHolder &operator=(ObjectHolder &&);
    ~ObjectHolder();

    explicit ObjectHolder(void *);

    explicit ObjectHolder(const std::string &);
    explicit ObjectHolder(const char *);
    explicit ObjectHolder(const void *, size_t);

    explicit ObjectHolder(bool);
    explicit ObjectHolder(double);
    explicit ObjectHolder(int);
    //// Guaranteed these do not change passed values
    explicit ObjectHolder(ObjectHolderMap_t &);
    explicit ObjectHolder(ObjectHolderList_t &);



    const void *GetObject() const;
    void *GetObject();

    std::string GetString() const;

    typedef std::pair<bool, double>    DoubleEntry_t;
    typedef std::pair<bool, bool>      BooleanEntry_t;
    typedef std::pair<bool, int>       IntegerEntry_t;
    typedef std::pair<bool, ptrdiff_t> LongEntry_t;
    DoubleEntry_t  GetDouble() const;
    BooleanEntry_t GetBoolean() const;
    IntegerEntry_t GetInteger() const;
    LongEntry_t    GetLong() const;
    bool           IsList() const;
    bool           IsCallable() const;
    bool           GetDoubleList(std::vector<double> &) const;
    bool           GetStringList(std::vector<std::string> &) const;
    bool           GetIntegerList(std::vector<int> &) const;
    bool           GetLongList(std::vector<ptrdiff_t> &) const;
    bool           GetUnsignedLongList(std::vector<size_t> &) const;

    bool empty() const
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

template <typename T>
inline ObjectHolder CreateObjectHolderList(const std::vector<T> &list)
{
  ObjectHolder result;
  const size_t length = list.size();
  ObjectHolderList_t objects(length);

  for (size_t i = 0; i < length; ++i)
  {
    objects[i] = ObjectHolder(list[i]);
  }

  return ObjectHolder(objects);
}

template <typename T>
inline ObjectHolder CreateDoubleObjectHolderList(const std::vector<T> &list)
{
  ObjectHolder result;
  const size_t length = list.size();
  ObjectHolderList_t objects(length);

  for (size_t i = 0; i < length; ++i)
  {
    objects[i] = ObjectHolder(static_cast<double>(list[i]));
  }

  return ObjectHolder(objects);
}

ObjectHolder CreateArrayObject(const char *s, const ObjectHolder &data_object);

template <typename T>
ObjectHolder CreatePODArray(const std::vector<T> &list);

template <typename T>
ObjectHolder CreateDoublePODArray(const std::vector<T> &list);

template <>
ObjectHolder CreateDoublePODArray(const std::vector<double> &list);

#endif

