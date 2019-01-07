/***
DEVSIM
Copyright 2013 Devsim LLC

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

#include "ObjectHolder.hh"
#include "dsAssert.hh"
#include <tcl.h>
#include <limits>

ObjectHolder::ObjectHolder() : object_(nullptr)
{
}

ObjectHolder::ObjectHolder(const ObjectHolder &t) : object_(t.object_)
{
  if (object_)
  {
    Tcl_IncrRefCount(reinterpret_cast<Tcl_Obj *>(object_));
  }
}

ObjectHolder &ObjectHolder::operator=(const ObjectHolder &t)
{
  /// Need to stop referencing the current object
  if (this != &t)
  {
    if (object_)
    {
      Tcl_DecrRefCount(reinterpret_cast<Tcl_Obj *>(object_));
    }

    object_ = t.object_;
    if (object_)
    {
      Tcl_IncrRefCount(reinterpret_cast<Tcl_Obj *>(object_));
    }
  }

  return *this;
}

ObjectHolder::~ObjectHolder()
{
  if (object_)
  {
    Tcl_DecrRefCount(reinterpret_cast<Tcl_Obj *>(object_));
  }
}

void ObjectHolder::clear()
{
  if (object_)
  {
    Tcl_DecrRefCount(reinterpret_cast<Tcl_Obj *>(object_));
  }
  object_ = nullptr;
}

ObjectHolder::ObjectHolder(void *t) : object_(t)
{
  if (object_)
  {
    Tcl_IncrRefCount(reinterpret_cast<Tcl_Obj *>(object_));
  }
}

std::string ObjectHolder::GetString() const
{
  std::string ret;
  if (object_)
  {
    ret = Tcl_GetStringFromObj(reinterpret_cast<Tcl_Obj *>(object_), nullptr);
  }
  return ret;
}

ObjectHolder::DoubleEntry_t ObjectHolder::GetDouble() const
{
  bool   ok = false;
  double val(0.0);

  if (object_)
  {
    ok = (TCL_OK == Tcl_GetDoubleFromObj(nullptr, reinterpret_cast<Tcl_Obj *>(object_), &val));
  }

  return std::make_pair(ok, val);
}

ObjectHolder::BooleanEntry_t ObjectHolder::GetBoolean() const
{
  bool   ok = false;
  bool   val= false;

  if (object_)
  {
    int ret = 0;
    ok = (TCL_OK == Tcl_GetBooleanFromObj(nullptr, reinterpret_cast<Tcl_Obj *>(object_), &ret));
    val = (ret == 1);
  }

  return std::make_pair(ok, val);
}


ObjectHolder::IntegerEntry_t ObjectHolder::GetInteger() const
{
  bool   ok = false;
  int    val= 0;

  if (object_)
  {
    ok = (TCL_OK == Tcl_GetIntFromObj(nullptr, reinterpret_cast<Tcl_Obj *>(object_), &val));
  }

  return std::make_pair(ok, val);
}

ObjectHolder::LongEntry_t ObjectHolder::GetLong() const
{
  bool      ok = false;

// a long on windows 64 bit is 32 bits
// on windows, compiler error
// note: Types pointed to are unrelated; conversion requires reinterpret_cast, C-style cast or function-style cast
#if _WIN32
  long val= 0;
#else
  ptrdiff_t val= 0;
#endif

  if (object_)
  {
    ok = (TCL_OK == Tcl_GetLongFromObj(nullptr, reinterpret_cast<Tcl_Obj *>(object_), &val));
  }

  return std::make_pair(ok, val);
}

bool ObjectHolder::IsList() const
{
  bool ok = false;
  if (object_)
  {
    int len;
    int ret = Tcl_ListObjLength(nullptr, reinterpret_cast<Tcl_Obj *>(object_), &len);
    ok = (ret == TCL_OK);
  }
  return ok;
}

bool ObjectHolder::GetListOfObjects(std::vector<ObjectHolder> &objs) const
{
  bool ok = false;
  objs.clear();

  Tcl_Obj *ptr = nullptr;
  if (IsList())
  {
    int len = 0;
    int ret = Tcl_ListObjLength(nullptr, reinterpret_cast<Tcl_Obj *>(object_), &len);

    ok = (ret == TCL_OK);
    if (ok)
    {
      objs.reserve(len);
      for (int i = 0; i < len; ++i)
      {
        ret = Tcl_ListObjIndex(nullptr, reinterpret_cast<Tcl_Obj *>(object_), i, &ptr);
        dsAssert(ret == TCL_OK, "UNEXPECTED");
        objs.push_back(ObjectHolder(ptr));
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

bool ObjectHolder::GetLongList(std::vector<ptrdiff_t> &values) const
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
      const ObjectHolder::LongEntry_t &ent = objs[i].GetLong();
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

bool ObjectHolder::GetUnsignedLongList(std::vector<size_t> &values) const
{
  std::vector<ptrdiff_t> lvalues;

  bool ok = this->GetLongList(lvalues);

  if (ok)
  {
    values.resize(lvalues.size());
    for (size_t i = 0; i < values.size(); ++i)
    {
      // no error checking for now
      values[i] = lvalues[i];
    }
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
  object_ = Tcl_NewStringObj(s.c_str(), s.size());
  Tcl_IncrRefCount(reinterpret_cast<Tcl_Obj *>(object_));
}

ObjectHolder::ObjectHolder(double v)
{
  object_ = Tcl_NewDoubleObj(v);
  Tcl_IncrRefCount(reinterpret_cast<Tcl_Obj *>(object_));
}

ObjectHolder::ObjectHolder(int v)
{
  object_ = Tcl_NewIntObj(v);
  Tcl_IncrRefCount(reinterpret_cast<Tcl_Obj *>(object_));
}


ObjectHolder::ObjectHolder(ObjectHolderList_t &list)
{
  Tcl_Obj *listPtr = nullptr;
  listPtr = Tcl_NewListObj(0, nullptr);
  Tcl_IncrRefCount(listPtr);
  for (ObjectHolderList_t::iterator it = list.begin(); it != list.end(); ++it)
  {
    Tcl_Obj *obj = reinterpret_cast<Tcl_Obj *>(it->GetObject());
    Tcl_IncrRefCount(obj);
    Tcl_ListObjAppendElement(nullptr, listPtr, obj);
  }
  object_ = listPtr;
}

ObjectHolder::ObjectHolder(ObjectHolderMap_t &map)
{
  Tcl_Obj *mapPtr = nullptr;
  mapPtr = Tcl_NewDictObj();
  Tcl_IncrRefCount(mapPtr);
  for (ObjectHolderMap_t::iterator it = map.begin(); it != map.end(); ++it)
  {
    const std::string s = it->first;
    Tcl_Obj *key = Tcl_NewStringObj(s.c_str(), s.size());
    Tcl_Obj *val = reinterpret_cast<Tcl_Obj *>((it->second).GetObject());
    Tcl_DictObjPut(nullptr, mapPtr, key, val);
  }
  object_ = mapPtr;
}


