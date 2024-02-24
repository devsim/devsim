/***
DEVSIM
Copyright 2017 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "EquationHolder.hh"
#include "Equation.hh"
#include "MatrixEntries.hh"
namespace {
template <typename T1, typename T2>
void ConvertVector(const std::vector<std::complex<T1>> &src, std::vector<std::complex<T2>> &dest)
{
  for (size_t i = 0; i < src.size(); ++i)
  {
    dest[i] = std::complex<T2>(static_cast<T2>(src[i].real()), static_cast<T2>(src[i].imag()));
  }
}

template <typename T1, typename T2>
void ConvertVector(const std::vector<T1> &src, std::vector<T2> &dest)
{
  for (size_t i = 0; i < src.size(); ++i)
  {
    dest[i] = static_cast<T2>(src[i]);
  }
}

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
EquationHolder::EquationHolder(Equation<double> *eq) : double_(eq)
{
}

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
EquationHolder::EquationHolder(Equation<float128> *eq) : float128_(eq)
{
}
#endif

EquationHolder::~EquationHolder()
{
}

bool EquationHolder::operator==(const EquationHolder &eq) const
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

std::string EquationHolder::GetName() const
{
  std::string ret;
  if (double_)
  {
    ret = (*double_).GetName();
  }
#ifdef DEVSIM_EXTENDED_PRECISION
  else if (float128_)
  {
    ret = (*float128_).GetName();
  }
#endif
  return ret;
}

std::string EquationHolder::GetVariable() const
{
  std::string ret;
  if (double_)
  {
    ret = (*double_).GetVariable();
  }
#ifdef DEVSIM_EXTENDED_PRECISION
  else if (float128_)
  {
    ret = (*float128_).GetVariable();
  }
#endif
  return ret;
}

template <typename DoubleType>
DoubleType EquationHolder::GetRelError() const
{
  DoubleType ret = 0.0;
  if (double_)
  {
    ret = (*double_).GetRelError();
  }
#ifdef DEVSIM_EXTENDED_PRECISION
  else if (float128_)
  {
    ret = static_cast<DoubleType>((*float128_).GetRelError());
  }
#endif
  return ret;
}

template <typename DoubleType>
DoubleType EquationHolder::GetAbsError() const
{
  DoubleType ret = 0.0;
  if (double_)
  {
    ret = (*double_).GetAbsError();
  }
#ifdef DEVSIM_EXTENDED_PRECISION
  else if (float128_)
  {
    ret = static_cast<DoubleType>((*float128_).GetAbsError());
  }
#endif
  return ret;
}

size_t EquationHolder::GetAbsErrorNodeIndex() const
{
  size_t ret{};
  if (double_)
  {
    ret = (*double_).GetAbsErrorNodeIndex();
  }
#ifdef DEVSIM_EXTENDED_PRECISION
  else if (float128_)
  {
    ret = (*float128_).GetAbsErrorNodeIndex();
  }
#endif
  return ret;
}

size_t EquationHolder::GetRelErrorNodeIndex() const
{
  size_t ret{};
  if (double_)
  {
    ret = (*double_).GetRelErrorNodeIndex();
  }
#ifdef DEVSIM_EXTENDED_PRECISION
  else if (float128_)
  {
    ret = (*float128_).GetRelErrorNodeIndex();
  }
#endif
  return ret;
}

template <>
void EquationHolder::Update(NodeModel &nm, const std::vector<double> &v) const
{
  if (double_)
  {
    (*double_).Update(nm, v);
  }
#ifdef DEVSIM_EXTENDED_PRECISION
  else if (float128_)
  {
    std::vector<float128> vv(v.size());
    ConvertVector(v, vv);
    (*float128_).Update(nm, vv);
  }
#endif
}

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
void EquationHolder::Update(NodeModel &nm, const std::vector<float128> &v) const
{
  if (double_)
  {
    std::vector<double> vv(v.size());
    ConvertVector(v, vv);
    (*double_).Update(nm, vv);
  }
  else if (float128_)
  {
    (*float128_).Update(nm, v);
  }
}
#endif

template <>
void EquationHolder::ACUpdate<double>(NodeModel &nm, const dsMath::ComplexDoubleVec_t<double> &v) const
{
  if (double_)
  {
    (*double_).ACUpdate(nm, v);
  }
#ifdef DEVSIM_EXTENDED_PRECISION
  else if (float128_)
  {
    dsMath::ComplexDoubleVec_t<float128> vv(v.size());
    ConvertVector(v, vv);
    (*float128_).ACUpdate(nm, vv);
  }
#endif
}

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
void EquationHolder::ACUpdate<float128>(NodeModel &nm, const dsMath::ComplexDoubleVec_t<float128> &v) const
{
  if (double_)
  {
    dsMath::ComplexDoubleVec_t<double> vv(v.size());
    ConvertVector(v, vv);
    (*double_).ACUpdate(nm, vv);
  }
  else if (float128_)
  {
    (*float128_).ACUpdate(nm, v);
  }
}
#endif

template <>
void EquationHolder::NoiseUpdate<double>(const std::string &nm, const std::vector<PermutationEntry> &permvec, const dsMath::ComplexDoubleVec_t<double> &rhs) const
{
  if (double_)
  {
    (*double_).NoiseUpdate(nm, permvec, rhs);
  }
#ifdef DEVSIM_EXTENDED_PRECISION
  else if (float128_)
  {
    dsMath::ComplexDoubleVec_t<float128> vv(rhs.size());
    ConvertVector(rhs, vv);
    (*float128_).NoiseUpdate(nm, permvec, vv);
  }
#endif
}

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
void EquationHolder::NoiseUpdate<float128>(const std::string &nm, const std::vector<PermutationEntry> &permvec, const dsMath::ComplexDoubleVec_t<float128> &rhs) const
{
  if (double_)
  {
    dsMath::ComplexDoubleVec_t<double> vv(rhs.size());
    ConvertVector(rhs, vv);
    (*double_).NoiseUpdate(nm, permvec, vv);
  }
  else if (float128_)
  {
    (*float128_).NoiseUpdate(nm, permvec, rhs);
  }
}
#endif

template <>
void EquationHolder::Assemble(dsMath::RealRowColValueVec<double> &m, dsMath::RHSEntryVec<double> &v, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
  if (double_)
  {
    (*double_).Assemble(m, v, w, t);
  }
#ifdef DEVSIM_EXTENDED_PRECISION
  else if (float128_)
  {
    dsMath::RealRowColValueVec<float128> mm;
    dsMath::RHSEntryVec<float128>        vv;
    (*float128_).Assemble(mm, vv, w, t);
    ConvertRealRowColVec(mm, m);
    ConvertRHSEntryVec(vv, v);
  }
#endif
}

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
void EquationHolder::Assemble(dsMath::RealRowColValueVec<float128> &m, dsMath::RHSEntryVec<float128> &v, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
  if (double_)
  {
    dsMath::RealRowColValueVec<double> mm;
    dsMath::RHSEntryVec<double>        vv;
    (*double_).Assemble(mm, vv, w, t);
    ConvertRealRowColVec(mm, m);
    ConvertRHSEntryVec(vv, v);
  }
  else if (float128_)
  {
    (*float128_).Assemble(m, v, w, t);
  }
}
#endif

void EquationHolder::GetCommandOptions(std::map<std::string, ObjectHolder> &m) const
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

void EquationHolder::DevsimSerialize(std::ostream &o) const
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

template double EquationHolder::GetRelError() const;
template double EquationHolder::GetAbsError() const;

#ifdef DEVSIM_EXTENDED_PRECISION
template float128 EquationHolder::GetRelError() const;
template float128 EquationHolder::GetAbsError() const;
#endif

