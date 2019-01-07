/***
DEVSIM
Copyright 2013 Devsim LLC

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

#include "MathEval.hh"
#include "Bernoulli.hh"
#include "Fermi.hh"
#include "LogicalMath.hh"
#include "MiscMathFunc.hh"
#include "dsAssert.hh"
#include "GlobalData.hh"
#include "AdditionalMath.hh"
#include "kahan.hh"

#include "Interpreter.hh"

#include "MathPacket.hh"

#include "MathWrapper.hh"

#include <cmath>
using std::abs;
#include <sstream>
#include <map>

#if 0
#ifdef _WIN32
#include "boost/math/special_functions/erf.hpp"
#endif
#endif

template <> MathEval<double> *MathEval<double>::instance_ = nullptr;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template <> MathEval<float128> *MathEval<float128>::instance_ = nullptr;
#endif

namespace Eqomfp {
  template <typename T>
  struct TernaryTblEntry {
      const char *name;
      ternaryfuncptr<T> func;
      const char *desc;
  };

  template <typename T>
  struct QuaternaryTblEntry {
      const char *name;
      quaternaryfuncptr<T> func;
      const char *desc;
  };

  template <typename T>
  struct BinaryTblEntry {
      const char *name;
      binaryfuncptr<T> func;
      const char *desc;
  };

  template <typename T>
  struct UnaryTblEntry {
      const char *name;
      unaryfuncptr<T> func;
      const char *desc;
  };

  UnaryTblEntry<double> UnaryTable_double[] = {
  {"abs",       abs,         "abs(obj)   -- Absolute value"},
  {"exp",       exp,          "exp(obj)   -- Exponentiation with respect to e"},
  {"log",       log,          "log(obj)   -- Natural logarithm"},
  {"B",         Bernoulli,    "B(obj)     -- Bernoulli Function"},
  {"dBdx",      derBernoulli, "dBdx(obj)  -- derivative Bernoulli wrt arg"},
  {"step",      step,         "step(obj)  -- step function"},
  {"sgn",       sgn,          "sgn(obj)   -- sign function"},
  {"acosh",     acosh,        "acosh(obj)   -- inverse hyperbolic cosine function"},
  {"asinh",     asinh,        "asinh(obj)   -- inverse hyperbolic sine function"},
  {"atanh",     atanh,        "atanh(obj)   -- inverse hyperbolic tangent function"},
  {"erf",       erf,          "erf(obj)   -- error function"},
  {"erfc",      erfc,         "erfc(obj)  -- complementary error function"},
  {"derfdx",    derfdx,       "derfdx(obj)   -- derivative error function"},
  {"derfcdx",   derfcdx,      "derfcdx(obj)  -- derivative of complementary error function"},
  {"Fermi",     Fermi,        "Fermi(obj)  -- Fermi Integral"},
  {"dFermidx",  dFermidx,     "dFermidx(obj)  -- derivative of Fermi Integral"},
  {"InvFermi",     InvFermi,        "InvFermi(obj)  -- inverse of the Fermi Integral"},
  {"dInvFermidx",  dInvFermidx,     "dInvFermidx(obj)  -- derivative of InvFermi Integral"},
  {"!",  logical_not,     "!obj  -- Logical Not"},
  {"vec_sum",   vec_sum, "Vector Summation"},
  {"vec_max",   vec_max, "Vector Maximum"},
  {"vec_min",   vec_min, "Vector Minimum"},
  {nullptr, nullptr, nullptr}
  };

  BinaryTblEntry<double> BinaryTable_double[] = {
  {"min",  min,  "min(obj1, obj2)       -- minimum of obj1 and obj2"},
  {"max",  max,  "max(obj1, obj2)       -- maximum of obj1 and obj2"},
  {"&&",  logical_and,  "obj1 && obj2       -- logical and"},
  {"||",  logical_or,  "obj1 || obj2       -- logical or"},
  {"==",  logical_eq,  "obj1 == obj2       -- logical equal"},
  {"<",  logical_lt,  "obj1 < obj2       -- logical less"},
  {"<=",  logical_lte,  "obj1 <= obj2       -- logical less than equal"},
  {">",  logical_gt,  "obj1 > obj2       -- logical greater"},
  {">=",  logical_gte,  "obj1 >= obj2       -- logical greater than equal"},
  {nullptr, nullptr, nullptr}
  };

  TernaryTblEntry<double> TernaryTable_double[] = {
  {"ifelse",  ifelsefunc,  "ifelse(obj1, obj2, obj3) -- if (obj1) then (obj2) else (obj3)"},
  {"kahan3",  kahan3,  "kahan(obj1, obj2, obj3) -- kahan summation"},
  {nullptr, nullptr, nullptr}
  };

  QuaternaryTblEntry<double> QuaternaryTable_double[] = {
  {"dot2d",  dot2dfunc,  "dot2d(ax, ay, bx, by) -- ax*bx + ay*by"},
  {"kahan4",  kahan4,  "kahan(obj1, obj2, obj3, obj4) -- kahan summation"},
  {nullptr, nullptr, nullptr}
  };

#ifdef DEVSIM_EXTENDED_PRECISION
namespace eval128 {
float128 abs(float128 x)
{
  return boost::multiprecision::abs(x);
}
float128 exp(float128 x)
{
  return boost::multiprecision::exp(x);
}
float128 log(float128 x)
{
  return boost::multiprecision::log(x);
}

float128 Bernoulli(float128 x)
{
  return ::Bernoulli(static_cast<double>(x));
}

float128 derBernoulli(float128 x)
{
  return ::derBernoulli(static_cast<double>(x));
}

float128 Fermi(float128 x)
{
  return ::Fermi(static_cast<double>(x));
}

float128 dFermidx(float128 x)
{
  return ::dFermidx(static_cast<double>(x));
}

float128 InvFermi(float128 x)
{
  return ::InvFermi(static_cast<double>(x));
}

float128 dInvFermidx(float128 x)
{
  return ::dInvFermidx(static_cast<double>(x));
}

float128 acosh(float128 x)
{
  return std::acosh(static_cast<double>(x));
}

float128 asinh(float128 x)
{
  return std::asinh(static_cast<double>(x));
}

float128 atanh(float128 x)
{
  return std::atanh(static_cast<double>(x));
}


float128 erf(float128 x)
{
  return ::erf(static_cast<double>(x));
}

float128 erfc(float128 x)
{
  return ::erfc(static_cast<double>(x));
}


float128 derfdx(float128 x)
{
  return ::derfdx(static_cast<double>(x));
}


float128 derfcdx(float128 x)
{
  return ::derfcdx(static_cast<double>(x));
}
}



  UnaryTblEntry<float128> UnaryTable_float128[] = {
  {"abs",      eval128::abs,         "abs(obj)   -- Absolute value"},
  {"exp",      eval128::exp,          "exp(obj)   -- Exponentiation with respect to e"},
  {"log",      eval128::log,          "log(obj)   -- Natural logarithm"},
  {"B",        eval128::Bernoulli,    "B(obj)     -- Bernoulli Function"},
  {"dBdx",     eval128::derBernoulli, "dBdx(obj)  -- derivative Bernoulli wrt arg"},
  {"step",     step,         "step(obj)  -- step function"},
  {"sgn",      sgn,          "sgn(obj)   -- sign function"},
  {"acosh",    eval128::acosh,        "acosh(obj)   -- inverse hyperbolic cosine function"},
  {"asinh",    eval128::asinh,        "asinh(obj)   -- inverse hyperbolic sine function"},
  {"atanh",    eval128::atanh,        "atanh(obj)   -- inverse hyperbolic tangent function"},
  {"erf",      eval128::erf,          "erf(obj)   -- error function"},
  {"erfc",     eval128::erfc,         "erfc(obj)  -- complementary error function"},
  {"derfdx",   eval128::derfdx,       "derfdx(obj)   -- derivative error function"},
  {"derfcdx",  eval128::derfcdx,      "derfcdx(obj)  -- derivative of complementary error function"},
  {"Fermi",    eval128::Fermi,        "Fermi(obj)  -- Fermi Integral"},
  {"dFermidx", eval128::dFermidx,     "dFermidx(obj)  -- derivative of Fermi Integral"},
  {"InvFermi",     eval128::InvFermi,        "InvFermi(obj)  -- inverse of the Fermi Integral"},
  {"dInvFermidx",  eval128::dInvFermidx,     "dInvFermidx(obj)  -- derivative of InvFermi Integral"},
  {"!",  logical_not,     "!obj  -- Logical Not"},
  {"vec_sum",   vec_sum, "Vector Summation"},
  {"vec_max",   vec_max, "Vector Maximum"},
  {"vec_min",   vec_min, "Vector Minimum"},
  {nullptr, nullptr, nullptr}
  };

  BinaryTblEntry<float128> BinaryTable_float128[] = {
  {"min",  min,  "min(obj1, obj2)       -- minimum of obj1 and obj2"},
  {"max",  max,  "max(obj1, obj2)       -- maximum of obj1 and obj2"},
  {"&&",  logical_and,  "obj1 && obj2       -- logical and"},
  {"||",  logical_or,  "obj1 || obj2       -- logical or"},
  {"==",  logical_eq,  "obj1 == obj2       -- logical equal"},
  {"<",  logical_lt,  "obj1 < obj2       -- logical less"},
  {"<=",  logical_lte,  "obj1 <= obj2       -- logical less than equal"},
  {">",  logical_gt,  "obj1 > obj2       -- logical greater"},
  {">=",  logical_gte,  "obj1 >= obj2       -- logical greater than equal"},
    {nullptr, nullptr, nullptr}
  };
  TernaryTblEntry<float128> TernaryTable_float128[] = {
  {"ifelse",  ifelsefunc,  "ifelse(obj1, obj2, obj3) -- if (obj1) then (obj2) else (obj3)"},
  {"kahan3",  kahan3,  "kahan(obj1, obj2, obj3) -- kahan summation"},
    {nullptr, nullptr, nullptr}
  };
  QuaternaryTblEntry<float128> QuaternaryTable_float128[] = {
  {"dot2d",  dot2dfunc,  "dot2d(ax, ay, bx, by) -- ax*bx + ay*by"},
  {"kahan4",  kahan4,  "kahan(obj1, obj2, obj3, obj4) -- kahan summation"},
    {nullptr, nullptr, nullptr}
  };
#endif

struct Tables{
  template <typename DoubleType>
  static Eqomfp::UnaryTblEntry<DoubleType> &GetUnaryTable(size_t);
  template <typename DoubleType>
  static Eqomfp::BinaryTblEntry<DoubleType> &GetBinaryTable(size_t);
  template <typename DoubleType>
  static Eqomfp::TernaryTblEntry<DoubleType> &GetTernaryTable(size_t);
  template <typename DoubleType>
  static Eqomfp::QuaternaryTblEntry<DoubleType> &GetQuaternaryTable(size_t);
};

template <>
Eqomfp::UnaryTblEntry<double> &Tables::GetUnaryTable(size_t i)
{
  return UnaryTable_double[i];
}
template <>
Eqomfp::BinaryTblEntry<double> &Tables::GetBinaryTable(size_t i)
{
  return BinaryTable_double[i];
}
template <>
Eqomfp::TernaryTblEntry<double> &Tables::GetTernaryTable(size_t i)
{
  return TernaryTable_double[i];
}
template <>
Eqomfp::QuaternaryTblEntry<double> &Tables::GetQuaternaryTable(size_t i)
{
  return QuaternaryTable_double[i];
}

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
Eqomfp::UnaryTblEntry<float128> &Tables::GetUnaryTable(size_t i)
{
  return UnaryTable_float128[i];
}
template <>
Eqomfp::BinaryTblEntry<float128> &Tables::GetBinaryTable(size_t i)
{
  return BinaryTable_float128[i];
}
template <>
Eqomfp::TernaryTblEntry<float128> &Tables::GetTernaryTable(size_t i)
{
  return TernaryTable_float128[i];
}
template <>
Eqomfp::QuaternaryTblEntry<float128> &Tables::GetQuaternaryTable(size_t i)
{
  return QuaternaryTable_float128[i];
}
#endif

}

template <typename DoubleType>
MathEval<DoubleType>::MathEval()
{
}

template <typename DoubleType>
MathEval<DoubleType>::~MathEval()
{
}

template <typename DoubleType>
MathEval<DoubleType> &MathEval<DoubleType>::GetInstance()
{
  if (!instance_)
  {
    instance_ = new MathEval();
    instance_->InitializeBuiltInMathFunc();
  }
  return *instance_;
}

template <typename DoubleType>
void MathEval<DoubleType>::DestroyInstance()
{
  delete instance_;
  instance_ = nullptr;
}

template <typename DoubleType>
void MathEval<DoubleType>::InitializeBuiltInMathFunc()
{
  for (size_t i = 0; Eqomfp::Tables::GetUnaryTable<DoubleType>(i).name != nullptr; ++i)
  {
    const std::string &name   = Eqomfp::Tables::GetUnaryTable<DoubleType>(i).name;
    Eqomfp::unaryfuncptr<DoubleType> func = Eqomfp::Tables::GetUnaryTable<DoubleType>(i).func;
    FuncPtrMap_[name]       = Eqomfp::MathWrapperPtr<DoubleType>(new Eqomfp::MathWrapper1<DoubleType>(name, func));
  }
  for (size_t i = 0; Eqomfp::Tables::GetBinaryTable<DoubleType>(i).name != nullptr; ++i)
  {
    const std::string &name   = Eqomfp::Tables::GetBinaryTable<DoubleType>(i).name;
    Eqomfp::binaryfuncptr<DoubleType> func = Eqomfp::Tables::GetBinaryTable<DoubleType>(i).func;
    FuncPtrMap_[name]       = Eqomfp::MathWrapperPtr<DoubleType>(new Eqomfp::MathWrapper2<DoubleType>(name, func));
  }
  for (size_t i = 0; Eqomfp::Tables::GetTernaryTable<DoubleType>(i).name != nullptr; ++i)
  {
    const std::string &name   = Eqomfp::Tables::GetTernaryTable<DoubleType>(i).name;
    Eqomfp::ternaryfuncptr<DoubleType> func = Eqomfp::Tables::GetTernaryTable<DoubleType>(i).func;
    FuncPtrMap_[name]       = Eqomfp::MathWrapperPtr<DoubleType>(new Eqomfp::MathWrapper3<DoubleType>(name, func));
  }
  for (size_t i = 0; Eqomfp::Tables::GetQuaternaryTable<DoubleType>(i).name != nullptr; ++i)
  {
    const std::string &name   = Eqomfp::Tables::GetQuaternaryTable<DoubleType>(i).name;
    Eqomfp::quaternaryfuncptr<DoubleType> func = Eqomfp::Tables::GetQuaternaryTable<DoubleType>(i).func;
    FuncPtrMap_[name]       = Eqomfp::MathWrapperPtr<DoubleType>(new Eqomfp::MathWrapper4<DoubleType>(name, func));
  }

  FuncPtrMap_["pow"] = Eqomfp::MathWrapperPtr<DoubleType>(new Eqomfp::PowWrapper<DoubleType>("pow"));
}

template <typename DoubleType>
DoubleType MathEval<DoubleType>::EvaluateMathFunc(const std::string &func, std::vector<DoubleType> &vals, std::string &error) const
{
  const size_t cnt = vals.size();
  DoubleType x = 0.0;

  if (tclMathFuncMap_.count(func))
  {
    static std::vector<const std::vector<DoubleType> *> vvals;
    vvals.resize(cnt);
    static std::vector<DoubleType> result(1);
    EvaluateTclMathFunc(func, vals, vvals, error, result);
    if (error.empty())
    {
      x = result[0];
    }
  }
  else if (FuncPtrMap_.count(func))
  {
    typename std::map<std::string, Eqomfp::MathWrapperPtr<DoubleType>>::const_iterator it = FuncPtrMap_.find(func);
    const Eqomfp::MathWrapper<DoubleType> &MyFunc = *(it->second);
    x = MyFunc.Evaluate(vals, error);
  }

  return x;
}

template <typename DoubleType>
void MathEval<DoubleType>::EvaluateTclMathFunc(const std::string &func, std::vector<DoubleType> &dvals, const std::vector<const std::vector<DoubleType> *> &vvals, std::string &error, std::vector<DoubleType> &result) const
{
  tclMathFuncMap_t::const_iterator it = tclMathFuncMap_.find(func);

  dsAssert(it != tclMathFuncMap_.end(), "UNEXPECTED");

  const size_t tclcount = it->second;

  const size_t cnt = vvals.size();

  if (tclcount == cnt)
  {
    Interpreter MyInterp;
    //// put the objects in here
    tclObjVector.resize(cnt + 1);

    tclObjVector[0] = ObjectHolder(func);

    size_t numvecs  = 0;
    size_t numelems = 0;
    for (size_t i = 0; i < cnt; ++i)
    {
      if (vvals[i] == nullptr)
      {
        tclObjVector[i + 1] = ObjectHolder(static_cast<double>(dvals[i]));
      }
      else
      {
        tclObjVector[i + 1].clear();
        ++numvecs;
        if (numelems == 0)
        {
          numelems = vvals[i]->size();
        }
      }
    }

    //// This means we just want to get the one value for constant arguments
    if (numelems == 0)
    {
      numelems = 1;
      result.resize(1);
    }

    for (size_t j = 0; j < numelems; ++j)
    {
      for (size_t i = 0; i < cnt; ++i)
      {
        bool isvec = vvals[i] != nullptr;

        if (isvec)
        {          
          tclObjVector[i+1] = ObjectHolder(static_cast<double>(vvals[i]->operator[](j)));
        }
      }

      bool ok = MyInterp.RunCommand(tclObjVector);

      if (ok)
      {
        ObjectHolder out = MyInterp.GetResult();
        ObjectHolder::DoubleEntry_t outval = out.GetDouble();
        ok = outval.first;
        if (ok)
        {
          result[j] = outval.second;
        }
        else
        {
          std::ostringstream os;
          os << "Could not convert " << out.GetString() << " to a DoubleType\n";
          error += os.str();
          break;
        }
      }
      else
      {
        error += MyInterp.GetErrorString();
        break;
      }
    }
  }
  else
  {
    std::ostringstream os;
    os << "function registered with \"" << tclcount << "\" arguments and \"" << cnt << "\" were provided: \"" << func << "\"";
    error += os.str();
  }
}

template <typename DoubleType>
void MathEval<DoubleType>::EvaluateMathFunc(const std::string &func, std::vector<DoubleType> &dvals, const std::vector<const std::vector<DoubleType> *> &vvals, std::string &error, std::vector<DoubleType> &result, size_t vlen) const
{
  result.resize(vlen);

  if (tclMathFuncMap_.count(func))
  {
    EvaluateTclMathFunc(func, dvals, vvals, error, result);
  }
  else if (FuncPtrMap_.count(func))
  {
    typename std::map<std::string, Eqomfp::MathWrapperPtr<DoubleType>>::const_iterator it = FuncPtrMap_.find(func);
    const Eqomfp::MathWrapper<DoubleType> &MyFunc = *(it->second);
    error += Eqomfp::MathPacketRun(MyFunc, dvals, vvals, result, vlen);
//    MyFunc.Evaluate(dvals, vvals, error, result, 0, vlen);
    //// TODO: in the future not call this function but the schedular
#if 0
    Eqomfp::MathPacket MyPacket(MyFunc, dvals, vvals, result, vlen);
    MyPacket(0, vlen);
    error += MyPacket.getErrorString();
    if (FPECheck::CheckFPE(MyPacket.getFPEFlag()))
    {
      //// Raise FPE in the main thread
      FPECheck::raiseFPE(MyPacket.getFPEFlag());
    }
#endif
  }
  else
  {
      std::ostringstream os;
      os << "could not find function \"" << func << "\"";
      error += os.str();
  }
}

template <typename DoubleType>
void MathEval<DoubleType>::AddTclMath(const std::string &funcname, size_t numargs)
{
  ///Really need to make sure number of arguments make sense in tcl api
  tclMathFuncMap_[funcname] = numargs;
}

template <typename DoubleType>
void MathEval<DoubleType>::RemoveTclMath(const std::string &funcname)
{
  /// Does this throw?
  tclMathFuncMap_.erase(funcname);
}

template class MathEval<double>;

#ifdef DEVSIM_EXTENDED_PRECISION
template class MathEval<float128>;
#endif


