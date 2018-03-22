#include "InterfaceEquationHolder.hh"
#include "InterfaceEquation.hh"
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
InterfaceEquationHolder::InterfaceEquationHolder(InterfaceEquation<double> *eq) : double_(eq)
{
}

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
InterfaceEquationHolder::InterfaceEquationHolder(InterfaceEquation<float128> *eq) : float128_(eq)
{
}
#endif

bool InterfaceEquationHolder::operator==(const InterfaceEquationHolder &eq) const
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

std::string InterfaceEquationHolder::GetName() const
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

std::string InterfaceEquationHolder::GetName0() const
{
  std::string ret;
  if (double_)
  {
    ret = (*double_).GetName0();
  }
#ifdef DEVSIM_EXTENDED_PRECISION
  if (float128_)
  {
    ret = (*float128_).GetName0();
  }
#endif
  return ret;
}

std::string InterfaceEquationHolder::GetName1() const
{
  std::string ret;
  if (double_)
  {
    ret = (*double_).GetName1();
  }
#ifdef DEVSIM_EXTENDED_PRECISION
  if (float128_)
  {
    ret = (*float128_).GetName1();
  }
#endif
  return ret;
}

template<>
void InterfaceEquationHolder::Assemble(dsMath::RealRowColValueVec<double> &m, dsMath::RHSEntryVec<double> &v, PermutationMap &p, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
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
void InterfaceEquationHolder::Assemble(dsMath::RealRowColValueVec<float128> &m, dsMath::RHSEntryVec<float128> &v, PermutationMap &p, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
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

void InterfaceEquationHolder::DevsimSerialize(std::ostream &o) const
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

void InterfaceEquationHolder::GetCommandOptions(std::map<std::string, ObjectHolder> &m) const
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

