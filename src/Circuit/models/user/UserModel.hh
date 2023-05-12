/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef USERMODEL_HH
#define USERMODEL_HH
#include <map>
#include <utility>
#include <string>
#include <vector>
#include <memory>

class InstanceModel;
class NodeKeeper;

//typedef InstanceModelPtr (*CreateUserModelPtr)(const char *name, const char ** const nodelist, const char ** const paramlist);
typedef InstanceModel * (*CreateUserModelPtr)(NodeKeeper *, const std::string &, const std::vector<std::string> &);

// Also contains the number of nodes
typedef std::map<std::string, CreateUserModelPtr > UserModelList;

namespace UserModel {
InstanceModel *instantiateModel(NodeKeeper *, const std::string &, const std::string &, const std::vector<std::string> &, const std::vector<std::pair<std::string, double> > &);
UserModelList getUserModelList();
void generateModelList();
void loadModel(const std::string &, const std::string &);
void unloadModels();
}

#endif
