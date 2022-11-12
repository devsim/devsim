/***
DEVSIM
Copyright 2013 DEVSIM LLC

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
    return nodekeep_->AddNode(tmp, CircuitNodeType::INTERNAL);
}

/**
  MNA nodes are ones with no entry on their diagonal.  Right now, this is not an issue.
  It also creates a prefix to the name.
  */
CircuitNodePtr InstanceModel::AddMNANode(const char *name)
{
    std::string tmp = name_ + "." + name;
    return nodekeep_->AddNode(tmp, CircuitNodeType::MNA);
}

void InstanceModel::assembleDC(const std::vector<double> &sol, dsMath::RealRowColValueVec<double> &mat, dsMath::RHSEntryVec<double> &rhs)
{
  this->assembleDC_impl(sol, mat, rhs);
}

void InstanceModel::assembleTran(const double scl, const std::vector<double> &sol, dsMath::RealRowColValueVec<double> *mat, dsMath::RHSEntryVec<double> &rhs)
{
  this->assembleTran_impl(scl, sol, mat, rhs);
}

void InstanceModel::assembleACRHS(std::vector<std::pair<size_t, std::complex<double> > > &rhs)
{
  this->assembleACRHS_impl(rhs);
}

