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

#ifndef USERMODEL_HH
#define USERMODEL_HH
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class InstanceModel;
class NodeKeeper;

// typedef InstanceModelPtr (*CreateUserModelPtr)(const char *name, const char
// ** const nodelist, const char ** const paramlist);
typedef InstanceModel *(*CreateUserModelPtr)(NodeKeeper *, const std::string &,
                                             const std::vector<std::string> &);

// Also contains the number of nodes
typedef std::map<std::string, CreateUserModelPtr> UserModelList;

namespace UserModel {
InstanceModel *
instantiateModel(NodeKeeper *, const std::string &, const std::string &,
                 const std::vector<std::string> &,
                 const std::vector<std::pair<std::string, double>> &);
UserModelList getUserModelList();
void generateModelList();
void loadModel(const std::string &, const std::string &);
void unloadModels();
} // namespace UserModel

#endif
