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

#include "CommandHandler.hh"

void CommandHandler::SetStringResult(const std::string &v)
{
  ObjectHolder obj(v);
  SetObjectResult(obj);
}

void CommandHandler::SetDoubleResult(double v)
{
  ObjectHolder obj(v);
  SetObjectResult(obj);
}

void CommandHandler::SetIntResult(int v)
{
  ObjectHolder obj(v);
  SetObjectResult(obj);
}

void CommandHandler::SetListResult(ObjectHolderList_t &list)
{
  ObjectHolder obj(list);
  SetObjectResult(obj);
}

void CommandHandler::SetMapResult(ObjectHolderMap_t &map)
{
  ObjectHolder obj(map);
  SetObjectResult(obj);
}

namespace {
template <typename T>
ObjectHolder CreateListResult(const std::vector<T> &list)
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
}


void CommandHandler::SetStringListResult(const std::vector<std::string> &list)
{
  SetObjectResult(CreateListResult(list));
}

void CommandHandler::SetDoubleListResult(const std::vector<double> &list)
{
  SetObjectResult(CreateListResult(list));
}

void CommandHandler::SetIntListResult(const std::vector<int> &list)
{
  SetObjectResult(CreateListResult(list));
}

