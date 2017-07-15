#include "InterfaceEquationHolder.hh"
#include "InterfaceEquation.hh"

InterfaceEquationHolder::InterfaceEquationHolder(InterfaceEquation<double> *eq) : double_(eq)
{
}

bool InterfaceEquationHolder::operator==(const InterfaceEquationHolder &eq) const
{
  return double_ == eq.double_;
}

const std::string &InterfaceEquationHolder::GetName() const
{
  return (*double_).GetName();
}

void InterfaceEquationHolder::Assemble(dsMath::RealRowColValueVec<double> &m, dsMath::RHSEntryVec<double> &v, PermutationMap &p, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
  (*double_).Assemble(m, v, p, w, t);
}

void InterfaceEquationHolder::DevsimSerialize(std::ostream &o) const
{
  (*double_).DevsimSerialize(o);
}

void InterfaceEquationHolder::GetCommandOptions(std::map<std::string, ObjectHolder> &m) const
{
  (*double_).GetCommandOptions(m);
}

