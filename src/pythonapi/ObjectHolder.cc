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

/// Python is insisting on defined _POSIX_C_SOURCE and _X_OPEN_SOURCE
#include "Python.h"
#include "ObjectHolder.hh"
#include "dsAssert.hh"
#include <limits>

ObjectHolder::ObjectHolder() : object_(NULL)
{
}

ObjectHolder::ObjectHolder(const ObjectHolder &t) : object_(t.object_)
{
  if (object_)
  {
    Py_INCREF(reinterpret_cast<PyObject *>(object_));
  }
}

ObjectHolder &ObjectHolder::operator=(const ObjectHolder &t)
{
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

ObjectHolder::~ObjectHolder()
{
  if (object_)
  {
    Py_DECREF(reinterpret_cast<PyObject *>(object_));
  }
}

void ObjectHolder::clear()
{
  if (object_)
  {
    Py_DECREF(reinterpret_cast<PyObject *>(object_));
  }
  object_ = NULL;
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

std::string ObjectHolder::GetString() const
{
  std::string ret;
  if (object_)
  {
    PyObject *strobj = PyObject_Str(reinterpret_cast<PyObject *>(object_));
    if (strobj)
    {
      ret = PyString_AsString(strobj);
      Py_DECREF(strobj);
    }
  }
  return ret;
}

ObjectHolder::DoubleEntry_t ObjectHolder::GetDouble() const
{
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
    else if (PyInt_CheckExact(obj) || PyLong_CheckExact(obj))
    {
      return this->GetInteger();
    }
    else if (PyString_CheckExact(obj))
    {
      PyObject *fobj = PyFloat_FromString(obj, NULL);
      if (fobj)
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
  }

  return std::make_pair(ok, val);
}

ObjectHolder::BooleanEntry_t ObjectHolder::GetBoolean() const
{
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
  bool   ok = false;
  int    val= 0;

  if (object_)
  {
    long ret = 0;
    PyObject *obj = reinterpret_cast<PyObject *>(object_);
    if (PyInt_CheckExact(obj))
    {
      ok = true;
      ret = PyInt_AsLong(obj);
    }
    else if (PyLong_CheckExact(obj))
    {
      ok = true;
      ret = PyLong_AsLong(obj);
    }
    else if (PyString_CheckExact(obj))
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
          ret = PyLong_AsLong(iobj);
        }
        Py_DECREF(iobj);
      }
      PyErr_Clear();
    }
    if (ok)
    {
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
  }
  return std::make_pair(ok, val);
}

bool ObjectHolder::IsList() const
{
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

bool ObjectHolder::GetListOfObjects(ObjectHolderList_t &objs) const
{
  bool ok = false;
  objs.clear();

  if (IsList())
  {
    PyObject *obj = reinterpret_cast<PyObject *>(object_);
    if (PySequence_Check(obj))
    {
      ok = true;
      size_t len = PySequence_Size(obj);
      objs.resize(len);
      for (size_t i = 0; i < len; ++i)
      {
        //// this is a borrowed reference
        PyObject *lobj = PySequence_GetItem(obj, i);
        Py_INCREF(lobj);
        objs[i] = ObjectHolder(lobj);
      }
    }
  }

  return ok;
}

bool ObjectHolder::GetDoubleList(std::vector<double> &values) const
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
      const ObjectHolder::DoubleEntry_t &ent = objs[i].GetDouble();
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

bool ObjectHolder::GetHashKeys(ObjectHolderList_t &keys) const
{
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
  object_ = PyString_FromStringAndSize(s.c_str(), s.size());
//  Py_INCREF(reinterpret_cast<PyObject *>(object_));
}

ObjectHolder::ObjectHolder(double v)
{
  object_ = PyFloat_FromDouble(v);
//  Py_INCREF(reinterpret_cast<PyObject *>(object_));
}

ObjectHolder::ObjectHolder(int v)
{
  object_ = PyInt_FromLong(v);
//  Py_INCREF(reinterpret_cast<PyObject *>(object_));
}

ObjectHolder::ObjectHolder(ObjectHolderList_t &list)
{
  const size_t length = list.size();

  PyObject *list_object = PyTuple_New(length);

  for (size_t i = 0; i < length; ++i)
  {
    PyObject *ns = reinterpret_cast<PyObject *>(list[i].GetObject());
    Py_INCREF(ns);
    //// steals the reference
    PyTuple_SET_ITEM(list_object, i, ns);
  }
  
  object_ = list_object;
}

ObjectHolder::ObjectHolder(ObjectHolderMap_t &map)
{
  PyObject *map_object = PyDict_New();
  for (ObjectHolderMap_t::iterator it = map.begin(); it != map.end(); ++it)
  {
    const std::string &key = it->first;
    PyObject *val = reinterpret_cast<PyObject *>((it->second).GetObject());
    Py_INCREF(val);
    PyDict_SetItemString(map_object, key.c_str(), val);
  }
  Py_INCREF(map_object);
  object_ = map_object;
}


