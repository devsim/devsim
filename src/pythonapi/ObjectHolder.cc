/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

/// Python is insisting on defined _POSIX_C_SOURCE and _X_OPEN_SOURCE
#include "Python.h"
#include "ObjectHolder.hh"
#include "dsAssert.hh"
#include "ControlGIL.hh"
#include "dsMathTypes.hh"
#include <cassert>
#include <limits>
#include <algorithm>


ObjectHolder::ObjectHolder() : object_(nullptr)
{
}

ObjectHolder::ObjectHolder(const ObjectHolder &t) : object_(t.object_)
{
  EnsurePythonGIL gil;

  if (object_)
  {
    Py_INCREF(reinterpret_cast<PyObject *>(object_));
  }
}

ObjectHolder &ObjectHolder::operator=(const ObjectHolder &t)
{
  EnsurePythonGIL gil;

  /// Need to stop referencing the current object
  if (this != &t)
  {
    if (object_ != t.object_)
    {
      if (object_)
      {
        Py_DECREF(reinterpret_cast<PyObject *>(object_));
      }

      object_ = t.object_;
      if (object_)
      {
        Py_INCREF(reinterpret_cast<PyObject *>(object_));
      }
    }
  }

  return *this;
}

ObjectHolder::ObjectHolder(ObjectHolder &&t) : object_(t.object_)
{
  // stealing reference
  t.object_ = nullptr;
}

ObjectHolder &ObjectHolder::operator=(ObjectHolder &&t)
{
  std::swap(object_, t.object_);
  return *this;
}

ObjectHolder::~ObjectHolder()
{
  EnsurePythonGIL gil;

  if (object_)
  {
    Py_DECREF(reinterpret_cast<PyObject *>(object_));
  }
}

void ObjectHolder::clear()
{
  EnsurePythonGIL gil;

  if (object_)
  {
    Py_DECREF(reinterpret_cast<PyObject *>(object_));
  }
  object_ = nullptr;
}

ObjectHolder::ObjectHolder(void *t) : object_(t)
{
  //// Assume the reference already had a reference count increase
#if 0
  if (object_)
  {
    Py_INCREF(reinterpret_cast<PyObject *>(object_));
  }
#endif
}

ObjectHolder::ObjectHolder(bool tval)
{
  EnsurePythonGIL gil;

  PyObject *obj = nullptr;
  if (tval)
  {
    obj = Py_True;
  }
  else
  {
    obj = Py_False;
  }
  Py_INCREF(obj);
  object_ = obj;

}


namespace {

//https://stackoverflow.com/questions/5356773/python-get-string-representation-of-pyobject
std::string GetStringFromStringObject(PyObject *obj)
{
  EnsurePythonGIL gil;

  std::string ret;
  if (PyUnicode_CheckExact(obj))
  {
    PyObject *sobj = PyUnicode_AsUTF8String(obj);
    ret = PyBytes_AsString(sobj);
    Py_DECREF(sobj);
  }
  else if (PyBytes_CheckExact(obj))
  {
    ret = PyBytes_AsString(obj);
  }
  return ret;
}

}

std::string ObjectHolder::GetString() const
{
  EnsurePythonGIL gil;

  std::string ret;
  if (object_)
  {
    PyObject *obj = reinterpret_cast<PyObject *>(object_);
    ret = GetStringFromStringObject(obj);

    if (ret.empty())
    {
      PyObject *strobj = PyObject_Str(obj);
      ret = GetStringFromStringObject(strobj);
      Py_DECREF(strobj);
    }
  }
  return ret;
}

ObjectHolder::DoubleEntry_t ObjectHolder::GetDouble() const
{
  EnsurePythonGIL gil;

  bool   ok = false;
  double val(0.0);

  if (object_)
  {
    PyObject *obj = reinterpret_cast<PyObject *>(object_);
    if (PyFloat_CheckExact(obj))
    {
      ok  = true;
      val = PyFloat_AsDouble(obj);
    }
    else if (auto *fobj = PyNumber_Float(obj))
    {
      ok = true;
      val = PyFloat_AsDouble(fobj);
      Py_DECREF(fobj);
    }
    else
    {
      PyErr_Clear();
    }
  }

  return std::make_pair(ok, val);
}

ObjectHolder::BooleanEntry_t ObjectHolder::GetBoolean() const
{
  EnsurePythonGIL gil;

  bool   ok = false;
  bool   val= false;

  if (object_)
  {
    PyObject *obj = reinterpret_cast<PyObject *>(object_);
    int ret = PyObject_IsTrue(obj);
    if (ret != -1)
    {
      ok  = true;
      val = (ret == 1);
    }
  }

  return std::make_pair(ok, val);
}


ObjectHolder::IntegerEntry_t ObjectHolder::GetInteger() const
{
  EnsurePythonGIL gil;

  bool   ok = true;
  int    val= 0;

  ObjectHolder::LongEntry_t lent = this->GetLong();

  if (lent.first)
  {
    ptrdiff_t ret = lent.second;
    if ((ret >= INT_MIN) && (ret <= INT_MAX))
    {
      ok = true;
      val = ret;
    }
    else
    {
      ok = false;
    }
  }
  return std::make_pair(ok, val);
}

ObjectHolder::LongEntry_t ObjectHolder::GetLong() const
{
  EnsurePythonGIL gil;

  bool      ok =false;
  ptrdiff_t val=0;

  // a long on windows 64 bit is 32 bits
  if (object_)
  {
    PyObject *obj = reinterpret_cast<PyObject *>(object_);
    if (PyLong_CheckExact(obj))
    {
      ok = true;
      val = PyLong_AsLong(obj);
    }
    else if (PyUnicode_CheckExact(obj) || PyBytes_CheckExact(obj))
    {
      const std::string &s = this->GetString();
      PyErr_Clear();
      char *p;
      PyObject *iobj = PyLong_FromString(const_cast<char *>(s.c_str()), &p, 10);
      if (iobj)
      {
        if ((*p) == 0)
        {
          ok = true;
          val = PyLong_AsLong(iobj);
        }
        Py_DECREF(iobj);
      }
      PyErr_Clear();
    }
  }
  return std::make_pair(ok, val);
}

bool ObjectHolder::IsList() const
{
  EnsurePythonGIL gil;

  bool ok = false;
  if (object_)
  {
    PyObject *obj = reinterpret_cast<PyObject *>(object_);
    if (PySequence_Check(obj))
    {
      ok = true;
    }
  }
  return ok;
}

bool ObjectHolder::IsCallable() const
{
  EnsurePythonGIL gil;

  bool ok = false;
  if (object_)
  {
    PyObject *obj = reinterpret_cast<PyObject *>(object_);
    if (PyCallable_Check(obj))
    {
      ok = true;
    }
  }
  return ok;
}

bool ObjectHolder::GetListOfObjects(ObjectHolderList_t &objs) const
{
  EnsurePythonGIL gil;

  bool ok = false;
  objs.clear();

  PyObject *obj = reinterpret_cast<PyObject *>(object_);
  if (obj && PySequence_Check(obj))
  {
    ok = true;
    size_t len = PySequence_Size(obj);
    objs.resize(len);
    for (size_t i = 0; i < len; ++i)
    {
      //// this is a new reference
      PyObject *lobj = PySequence_GetItem(obj, i);
      //Py_INCREF(lobj);
      objs[i] = ObjectHolder(lobj);
    }
  }

  return ok;
}

namespace
{

template <typename T>
struct pod_info;

template <>
struct pod_info<std::complex<double>>
{
  [[maybe_unused]] constexpr static auto pmf = nullptr;
  [[maybe_unused]] constexpr static const char *ptype = "d";
};

template <>
struct pod_info<double>
{
  constexpr static auto pmf = &ObjectHolder::GetDouble;
  constexpr static const char *ptype = "d";
};

template <>
struct pod_info<int>
{
  constexpr static auto pmf = &ObjectHolder::GetInteger;
  constexpr static const char *ptype = "i";
};

#if defined(_WIN32) || (UINTPTR_MAX > UINT_MAX)
template <>
struct pod_info<ptrdiff_t>
{
  constexpr static auto pmf = &ObjectHolder::GetLong;
};
#endif

template <typename T>
bool GetFromList(const ObjectHolder &oh, std::vector<T> &values)
{
  bool ok = false;
  values.clear();
  ObjectHolderList_t objs;
  ok = oh.GetListOfObjects(objs);
  if (ok)
  {
    values.resize(objs.size());
    ok = true;
    dsAssert(pod_info<T>::pmf, "helper function not implemented");
    for (size_t i = 0; i < objs.size(); ++i)
    {
      const auto &ent = (objs[i].*pod_info<T>::pmf)();
      if (ent.first)
      {
        values[i] = ent.second;
      }
      else
      {
        values.clear();
        ok = false;
        break;
      }
    }
  }
  return ok;
}

void GetArrayInfo(const ObjectHolder &input, std::string &typecode, long &itemsize, ObjectHolder &bytes)
{
  EnsurePythonGIL gil;

  typecode.clear();
  itemsize = 0;
  bytes.clear();

  PyObject *obj = reinterpret_cast<PyObject *>(const_cast<void *>(input.GetObject()));

  if (!obj)
  {
    return;
  }

  if (PyBytes_Check(obj))
  {
    bytes = input;
  }
  else
  {

    ObjectHolder bytecall;

    if (PyObject_HasAttrString(obj, "tobytes"))
    {
      bytecall = ObjectHolder(PyObject_GetAttrString(obj, "tobytes"));

      // these do not exist without tobytes
      if (PyObject_HasAttrString(obj, "typecode"))
      {
        ObjectHolder obj_typecode(PyObject_GetAttrString(obj, "typecode"));
        typecode = obj_typecode.GetString();
      }

      if (PyObject_HasAttrString(obj, "itemsize"))
      {
        ObjectHolder obj_itemsize(PyObject_GetAttrString(obj, "itemsize"));
        itemsize = obj_itemsize.GetLong().second;
      }
    }

    if (bytecall.IsCallable())
    {
      bytes = ObjectHolder(PyObject_CallObject(reinterpret_cast<PyObject *>(bytecall.GetObject()), nullptr));
      PyErr_Clear();
    }
  }
}

template <typename T>
void GetBytesAsVector(ObjectHolder &bytes, std::vector<T> &result)
{
  EnsurePythonGIL gil;

  char *data = nullptr;
  Py_ssize_t length = 0;

  int r = PyBytes_AsStringAndSize(reinterpret_cast<PyObject *>(bytes.GetObject()), &data, &length);
  PyErr_Clear();
  if ((r != -1) && length > 0)
  {
    const size_t n = length / sizeof(T);
    result.resize(n);
    std::copy_n(reinterpret_cast<T *>(data), n, result.begin());
  }
}

template <typename T>
bool GetArrayFromBytes(const ObjectHolder &input, std::vector<T> &values, const std::string &expected_typecodes, long expected_itemsize)
{
  EnsurePythonGIL gil;

  values.clear();

  ObjectHolder bytes;
  std::string  typecode;
  long         itemsize = 0;

  GetArrayInfo(input, typecode, itemsize, bytes);

  if (bytes.empty())
  {
    return false;
  }

  bool typecode_found = !typecode.empty() && (expected_typecodes.find(typecode) != std::string::npos);

  if ((typecode.empty() && (itemsize == 0)) || ((typecode_found) && (itemsize == expected_itemsize)) ||  (typecode.empty() && (itemsize == expected_itemsize)))
  {
    GetBytesAsVector<T>(bytes, values);
  }

  return !values.empty();
}

template <typename T, typename U>
void convert_to_unsigned(const std::vector<T> &in, std::vector<U> &out)
{
  out.resize(in.size());
  for (size_t i = 0; i < in.size(); ++i)
  {
    out[i] = static_cast<U>(in[i]);
  }
}
}

bool ObjectHolder::GetDoubleList(std::vector<double> &values) const
{
  if (!GetArrayFromBytes<double>(*this, values, pod_info<double>::ptype, sizeof(double)))
  {
    return GetFromList<double>(*this, values);
  }
  return true;
}

bool ObjectHolder::GetComplexDoubleList(std::vector<std::complex<double>> &values) const
{
  bool ret = GetArrayFromBytes<std::complex<double>>(*this, values, pod_info<double>::ptype, sizeof(double));
  return ret;
}

bool ObjectHolder::GetIntegerList(std::vector<int> &values) const
{
#if defined(__MINGW32__) || defined(__MINGW64__) || defined(_WIN32) || (UINTPTR_MAX == UINT_MAX)
  static_assert(sizeof(long) == sizeof(int), "wrong sizeof(long)");
  const std::string search("iIlL");
#elif defined(__linux__) || defined(__APPLE__)
  static_assert(sizeof(long) == sizeof(long long), "wrong sizeof(long)");
  const std::string search("iI");
#else
#error "FIX TYPE"
#endif
  if (!GetArrayFromBytes<int>(*this, values, search, sizeof(int)))
  {
    return GetFromList<int>(*this, values);
  }
  return true;
}

bool ObjectHolder::GetLongList(std::vector<ptrdiff_t> &values) const
{
#if defined(__MINGW32__) || defined(__MINGW64__) || defined(_WIN32) || (UINTPTR_MAX == UINT_MAX)
  static_assert(sizeof(long) == sizeof(int), "wrong sizeof(long)");
  const std::string search("qQ");
#elif defined(__linux__) || defined(__APPLE__)
  static_assert(sizeof(long) == sizeof(long long), "wrong sizeof(long)");
  const std::string search("lLqQ");
#else
#error "FIX TYPE"
#endif

  if (!GetArrayFromBytes<ptrdiff_t>(*this, values, search, sizeof(ptrdiff_t)))
  {
    return GetFromList<ptrdiff_t>(*this, values);
  }
  return true;
}

bool ObjectHolder::GetUnsignedLongList(std::vector<size_t> &values) const
{
  std::vector<ptrdiff_t> lvalues;

  bool ok = this->GetLongList(lvalues);

  if (ok)
  {
    convert_to_unsigned(lvalues, values);
  }
  else
  {
    values.clear();
  }

  return ok;
}

bool ObjectHolder::GetStringList(std::vector<std::string> &values) const
{
  bool ok = false;
  values.clear();
  ObjectHolderList_t objs;
  ok = GetListOfObjects(objs);
  if (ok)
  {
    values.resize(objs.size());
    ok = true;
    for (size_t i = 0; i < objs.size(); ++i)
    {
      values[i] = objs[i].GetString();
    }
  }
  return ok;
}

bool ObjectHolder::GetHashKeys(ObjectHolderList_t &keys) const
{
  EnsurePythonGIL gil;

  bool ret = false;
  keys.clear();
  if (object_ && PyDict_CheckExact(object_))
  {
    ObjectHolder key_holder(PyDict_Keys(reinterpret_cast<PyObject *>(object_)));
    key_holder.GetListOfObjects(keys);
    ret = true;
  }
  return ret;
}

bool ObjectHolder::GetHashMap(ObjectHolderMap_t &hashmap) const
{
  EnsurePythonGIL gil;

  bool ret = false;
  hashmap.clear();
  if (object_ && PyDict_CheckExact(object_))
  {
    ret = true;
    ObjectHolder items_holder(PyDict_Items(reinterpret_cast<PyObject *>(object_)));
    ObjectHolderList_t items;
    items_holder.GetListOfObjects(items);
    ObjectHolderList_t item;
    for (size_t i = 0; i < items.size(); ++i)
    {
      items[i].GetListOfObjects(item);
      const std::string &key = item[0].GetString();
      ObjectHolder value = item[1];
      hashmap[key] = value;
    }
  }
  return ret;
}

const void *ObjectHolder::GetObject() const
{
  return object_;
}

void *ObjectHolder::GetObject()
{
  return object_;
}

ObjectHolder::ObjectHolder(const std::string &s)
{
  EnsurePythonGIL gil;

  object_ = PyUnicode_FromStringAndSize(s.c_str(), s.size());
}

ObjectHolder::ObjectHolder(const char *s)
{
  EnsurePythonGIL gil;

  object_ = PyUnicode_FromStringAndSize(s, strlen(s));
}

ObjectHolder::ObjectHolder(const void *s, size_t len)
{
  EnsurePythonGIL gil;

  object_ = PyByteArray_FromStringAndSize(static_cast<const char *>(s), len);
}


ObjectHolder::ObjectHolder(double v)
{
  EnsurePythonGIL gil;

  object_ = PyFloat_FromDouble(v);
}

ObjectHolder::ObjectHolder(int v)
{
  EnsurePythonGIL gil;

  object_ = PyLong_FromLong(v);
}

ObjectHolder::ObjectHolder(ObjectHolderList_t &list)
{
  EnsurePythonGIL gil;

  const size_t length = list.size();

  PyObject *list_object = PyTuple_New(length);

  for (size_t i = 0; i < length; ++i)
  {
    PyObject *ns = reinterpret_cast<PyObject *>(list[i].GetObject());
    Py_INCREF(ns);
    //// steals the reference
    PyTuple_SetItem(list_object, i, ns);
  }

  object_ = list_object;
}

ObjectHolder::ObjectHolder(ObjectHolderMap_t &map)
{
  EnsurePythonGIL gil;

  PyObject *map_object = PyDict_New();
  for (ObjectHolderMap_t::iterator it = map.begin(); it != map.end(); ++it)
  {
    const std::string &key = it->first;
    PyObject *val = reinterpret_cast<PyObject *>((it->second).GetObject());
    //// does not steal the reference
    //Py_INCREF(val);
    PyDict_SetItemString(map_object, key.c_str(), val);
  }
  //Py_INCREF(map_object);
  object_ = map_object;
}

ObjectHolder CreateArrayObject(const char *s, const ObjectHolder &data_object)
{
  EnsurePythonGIL gil;

  ObjectHolder array_mod(PyImport_ImportModule("array"));
  PyErr_Clear();
  dsAssert(!array_mod.empty(), "array module not available");

  ObjectHolder array_class(PyObject_GetAttrString(reinterpret_cast<PyObject *>(array_mod.GetObject()), "array"));
  PyErr_Clear();
  dsAssert(!array_class.empty(), "array.array not available");

  dsAssert(array_class.IsCallable(), "array.array is not callable");

  //https://docs.python.org/3/c-api/arg.html#c.Py_BuildValue
  ObjectHolder result(PyObject_CallFunction(reinterpret_cast<PyObject *>(array_class.GetObject()), "sO", s, data_object.GetObject()));
  PyErr_Clear();
  dsAssert(!result.empty(), "array.array not able to create data");

  return result;
}

template <typename T>
ObjectHolder CreatePODArray(const T *data, size_t length)
{
  return CreateArrayObject(pod_info<T>::ptype, ObjectHolder(data, length));
}

template <typename T>
ObjectHolder CreatePODArray(const std::vector<T> &list)
{
  const size_t length = list.size() * sizeof(T);
  return CreatePODArray(list.data(), length);
}

ObjectHolder CreateIntPODArray(const std::vector<int> &list)
{
  return CreatePODArray<int>(list);
}

ObjectHolder CreateDoublePODArray(const dsMath::DoubleVec_t<double> &list)
{
  return CreatePODArray<double>(list);
}

ObjectHolder CreateComplexDoublePODArray(const dsMath::ComplexDoubleVec_t<double> &list)
{
  return CreatePODArray<double>(reinterpret_cast<const double *>(list.data()), sizeof(std::complex<double>) * list.size());
}

#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
#include "dsMathTypes.hh"
ObjectHolder CreateDoublePODArray(const dsMath::DoubleVec_t<float128> &list)
{
  thread_local dsMath::DoubleVec_t<double> tmp;
  tmp.resize(list.size());

  std::transform(list.begin(), list.end(), tmp.begin(), [](auto &x){return static_cast<double>(x);});

  return CreatePODArray<double>(tmp);
}

ObjectHolder CreateComplexDoublePODArray(const dsMath::ComplexDoubleVec_t<float128> &list)
{
  thread_local dsMath::ComplexDoubleVec_t<double> tmp;
  tmp.resize(list.size());

  std::transform(list.begin(), list.end(), tmp.begin(), [](auto &x){return std::complex<double>(static_cast<double>(x.real()), static_cast<double>(x.imag()));});

  return CreateComplexDoublePODArray(tmp);
}

#endif

