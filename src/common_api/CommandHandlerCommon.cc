/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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

void CommandHandler::SetStringListResult(const std::vector<std::string> &list)
{
  SetObjectResult(CreateObjectHolderList(list));
}

void CommandHandler::SetDoubleListResult(const std::vector<double> &list)
{
  SetObjectResult(CreateObjectHolderList(list));
}

void CommandHandler::SetIntListResult(const std::vector<int> &list)
{
  SetObjectResult(CreateObjectHolderList(list));
}

