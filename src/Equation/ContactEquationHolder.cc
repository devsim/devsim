/***
DEVSIM
Copyright 2017 Devsim LLC

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

#include "ContactEquationHolder.hh"
#include "ContactEquation.hh"


ContactEquationHolder::ContactEquationHolder(ContactEquation<double> *eq) : double_(eq)
{
}

ContactEquationHolder::~ContactEquationHolder()
{
}

bool ContactEquationHolder::operator==(const ContactEquationHolder &eq) const
{
  return double_ == eq.double_;
}

double ContactEquationHolder::GetCharge() const
{
  return (*double_).GetCharge();
}

double ContactEquationHolder::GetCurrent() const
{
  return (*double_).GetCurrent();
}

const std::string &ContactEquationHolder::GetName() const
{
  return (*double_).GetName();
}

const std::string &ContactEquationHolder::GetVariable() const
{
  return (*double_).GetVariable();
}

void ContactEquationHolder::UpdateContact() const
{
  (*double_).UpdateContact();
}

void ContactEquationHolder::Assemble(dsMath::RealRowColValueVec<double> &m, dsMath::RHSEntryVec<double> &v, PermutationMap &p, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
  (*double_).Assemble(m, v, p, w, t);
}

void ContactEquationHolder::DevsimSerialize(std::ostream &o) const
{
  (*double_).DevsimSerialize(o);
}

void ContactEquationHolder::GetCommandOptions(std::map<std::string, ObjectHolder> &m) const
{
  (*double_).GetCommandOptions(m);
}

