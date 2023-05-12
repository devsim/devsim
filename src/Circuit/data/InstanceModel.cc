/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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

