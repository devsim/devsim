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

#include "CommandHandler.hh"

void CommandHandler::SetStringResult(const std::string &v) {
  ObjectHolder obj(v);
  SetObjectResult(obj);
}

void CommandHandler::SetDoubleResult(double v) {
  ObjectHolder obj(v);
  SetObjectResult(obj);
}

void CommandHandler::SetIntResult(int v) {
  ObjectHolder obj(v);
  SetObjectResult(obj);
}

void CommandHandler::SetListResult(ObjectHolderList_t &list) {
  ObjectHolder obj(list);
  SetObjectResult(obj);
}

void CommandHandler::SetMapResult(ObjectHolderMap_t &map) {
  ObjectHolder obj(map);
  SetObjectResult(obj);
}

void CommandHandler::SetStringListResult(const std::vector<std::string> &list) {
  SetObjectResult(CreateObjectHolderList(list));
}

void CommandHandler::SetDoubleListResult(const std::vector<double> &list) {
  SetObjectResult(CreateObjectHolderList(list));
}

void CommandHandler::SetIntListResult(const std::vector<int> &list) {
  SetObjectResult(CreateObjectHolderList(list));
}
