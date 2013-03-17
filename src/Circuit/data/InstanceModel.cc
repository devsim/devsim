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

#include "InstanceModel.hh"
#include "NodeKeeper.hh"

InstanceModel::~InstanceModel() {}

InstanceModel::InstanceModel(NodeKeeper *nkp, const char *name) : nodekeep_(nkp), name_(name)
{
}

/**
 *  Prefer the base member function since all the models
 *  shouldn't know the details of adding a node.
 */
CircuitNodePtr InstanceModel::AddCircuitNode(const char *name)
{
    return nodekeep_->AddNode(name);
}

CircuitNodePtr InstanceModel::AddInternalNode(const char *name)
{
    std::string tmp = name_ + "." + name;
    return nodekeep_->AddNode(tmp, CNT::INTERNAL);
}

/**
  MNA nodes are ones with no entry on their diagonal.  Right now, this is not an issue.
  It also creates a prefix to the name.
  */
CircuitNodePtr InstanceModel::AddMNANode(const char *name)
{
    std::string tmp = name_ + "." + name;
    return nodekeep_->AddNode(tmp, CNT::MNA);
}

