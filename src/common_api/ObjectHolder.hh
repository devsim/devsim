/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef OBJHOLDER_HH
#define OBJHOLDER_HH
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
#endif
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <cstddef>
#include <complex>


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
    bool           GetComplexDoubleList(std::vector<std::complex<double>> &) const;
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

ObjectHolder CreateIntPODArray(const std::vector<int> &list);

ObjectHolder CreateDoublePODArray(const std::vector<double>  &list);

ObjectHolder CreateComplexDoublePODArray(const std::vector<std::complex<double>>  &list);

#ifdef DEVSIM_EXTENDED_PRECISION
ObjectHolder CreateDoublePODArray(const std::vector<float128> &list);

ObjectHolder CreateComplexDoublePODArray(const std::vector<complex128> &list);
#endif
#endif

