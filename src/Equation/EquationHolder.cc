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

#include "EquationHolder.hh"
#include "Equation.hh"

EquationHolder::EquationHolder(Equation<double> *eq) : double_(eq)
{
}

EquationHolder::~EquationHolder()
{
}

bool EquationHolder::operator==(const EquationHolder &eq) const
{
  return double_ == eq.double_;
}

std::string EquationHolder::GetName() const
{
  return (*double_).GetName();
}

std::string EquationHolder::GetVariable() const
{
  return (*double_).GetVariable();
}

double EquationHolder::GetRelError() const
{
  return (*double_).GetRelError();
}

double EquationHolder::GetAbsError() const
{
  return double_->GetAbsError();
}

void EquationHolder::Update(NodeModel &nm, const std::vector<double> &v) const
{
  (*double_).Update(nm, v);
}

void EquationHolder::ACUpdate(NodeModel &nm, const std::vector<std::complex<double>> &v) const
{
  (*double_).ACUpdate(nm, v);
}

void EquationHolder::NoiseUpdate(const std::string &nm, const std::vector<size_t> &permvec, const std::vector<std::complex<double> > &rhs) const
{
  (*double_).NoiseUpdate(nm, permvec, rhs);
}

void EquationHolder::Assemble(dsMath::RealRowColValueVec<double> &m, dsMath::RHSEntryVec<double> &v, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
  (*double_).Assemble(m, v, w, t);
}

void EquationHolder::GetCommandOptions(std::map<std::string, ObjectHolder> &m) const
{
  (*double_).GetCommandOptions(m);
}

void EquationHolder::DevsimSerialize(std::ostream &o) const
{
  (*double_).DevsimSerialize(o);
}



