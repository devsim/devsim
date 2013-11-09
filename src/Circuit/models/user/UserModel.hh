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
