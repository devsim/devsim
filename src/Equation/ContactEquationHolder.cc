/***
DEVSIM
Copyright 2017 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "ContactEquationHolder.hh"
#include "ContactEquation.hh"
#include "MatrixEntries.hh"

namespace {
template <typename T1, typename T2>
void ConvertRealRowColVec(const dsMath::RealRowColValueVec<T1> &src, dsMath::RealRowColValueVec<T2> &dest)
{
  for (auto & x : src)
  {
      dest.push_back(dsMath::RealRowColVal<T2>(x.row, x.col, static_cast<T2>(x.val)));
  }
}

template <typename T1, typename T2>
void ConvertRHSEntryVec(const dsMath::RHSEntryVec<T1> &src, dsMath::RHSEntryVec<T2> &dest)
{
    for (auto &x : src)
    {
      dest.push_back(std::make_pair(x.first, static_cast<T2>(x.second)));
    }
}
}

template <>
ContactEquationHolder::ContactEquationHolder(ContactEquation<double> *eq) : double_(eq)
{
}

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
ContactEquationHolder::ContactEquationHolder(ContactEquation<float128> *eq) : float128_(eq)
{
}
#endif

ContactEquationHolder::~ContactEquationHolder()
{
}

bool ContactEquationHolder::operator==(const ContactEquationHolder &eq) const
{
  if (double_)
  {
    return double_ == eq.double_;
  }
#ifdef DEVSIM_EXTENDED_PRECISION
  else if (float128_)
  {
    return float128_ == eq.float128_;
  }
#endif
  return false;
}

template <typename DoubleType>
DoubleType ContactEquationHolder::GetCharge() const
{
  DoubleType ret = 0.0;
  if (double_)
  {
    ret = (*double_).GetCharge();
  }
#ifdef DEVSIM_EXTENDED_PRECISION
  if (float128_)
  {
    ret = static_cast<DoubleType>((*float128_).GetCharge());
  }
#endif
  return ret;
}

template <typename DoubleType>
DoubleType ContactEquationHolder::GetCurrent() const
{
  DoubleType ret = 0.0;
  if (double_)
  {
    ret = (*double_).GetCurrent();
  }
#ifdef DEVSIM_EXTENDED_PRECISION
  if (float128_)
  {
    ret = static_cast<DoubleType>((*float128_).GetCurrent());
  }
#endif
  return ret;
}

std::string ContactEquationHolder::GetName() const
{
  std::string ret;
  if (double_)
  {
    ret = (*double_).GetName();
  }
#ifdef DEVSIM_EXTENDED_PRECISION
  if (float128_)
  {
    ret = (*float128_).GetName();
  }
#endif
  return ret;
}

void ContactEquationHolder::UpdateContact() const
{
  if (double_)
  {
    (*double_).UpdateContact();
  }
#ifdef DEVSIM_EXTENDED_PRECISION
  if (float128_)
  {
    (*float128_).UpdateContact();
  }
#endif
}

template<>
void ContactEquationHolder::Assemble(dsMath::RealRowColValueVec<double> &m, dsMath::RHSEntryVec<double> &v, PermutationMap &p, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
  if (double_)
  {
    (*double_).Assemble(m, v, p, w, t);
  }
#ifdef DEVSIM_EXTENDED_PRECISION
  else if (float128_)
  {
    dsMath::RealRowColValueVec<float128> mm;
    dsMath::RHSEntryVec<float128>        vv;
    (*float128_).Assemble(mm, vv, p, w, t);
    ConvertRealRowColVec(mm, m);
    ConvertRHSEntryVec(vv, v);
  }
#endif
}

#ifdef DEVSIM_EXTENDED_PRECISION
template<>
void ContactEquationHolder::Assemble(dsMath::RealRowColValueVec<float128> &m, dsMath::RHSEntryVec<float128> &v, PermutationMap &p, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
  if (double_)
  {
    dsMath::RealRowColValueVec<double> mm;
    dsMath::RHSEntryVec<double>        vv;
    (*double_).Assemble(mm, vv, p, w, t);
    ConvertRealRowColVec(mm, m);
    ConvertRHSEntryVec(vv, v);
  }
  else if (float128_)
  {
    (*float128_).Assemble(m, v, p, w, t);
  }
}
#endif

void ContactEquationHolder::DevsimSerialize(std::ostream &o) const
{
  if (double_)
  {
    (*double_).DevsimSerialize(o);
  }
#ifdef DEVSIM_EXTENDED_PRECISION
  else if (float128_)
  {
    (*float128_).DevsimSerialize(o);
  }
#endif
}

void ContactEquationHolder::GetCommandOptions(std::map<std::string, ObjectHolder> &m) const
{
  if (double_)
  {
    (*double_).GetCommandOptions(m);
  }
#ifdef DEVSIM_EXTENDED_PRECISION
  else if (float128_)
  {
    (*float128_).GetCommandOptions(m);
  }
#endif
}

template double ContactEquationHolder::GetCharge() const;
template double ContactEquationHolder::GetCurrent() const;

#ifdef DEVSIM_EXTENDED_PRECISION
template float128 ContactEquationHolder::GetCharge() const;
template float128 ContactEquationHolder::GetCurrent() const;
#endif


