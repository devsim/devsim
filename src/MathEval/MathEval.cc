/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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
#include "GaussFermi.hh"

#include <cmath>
using std::abs;
#include <sstream>
#include <map>
#include "BoostSpecialFunctions.hh"
using my_policy::use_errno;

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


namespace eval64 {
#if defined(__MINGW32__) || defined(__MINGW64__)
#include "FPECheck.hh"
double log(double x)
{
  if (x > 0.0)
  {
    return std::log(x);
  }

  return FPECheck::ManualCheckAndRaiseFPE(std::log(x));
}

double exp(double x)
{
  if (x < 709.8)
  {
    return std::exp(x);
  }

  return FPECheck::ManualCheckAndRaiseFPE(std::exp(x));
}
#else
using std::log;
using std::exp;
#endif

double erf(double x)
{
  return boost::math::erf(x, use_errno());
}

double erfc(double x)
{
  return boost::math::erfc(x, use_errno());
}


using ::derfdx;
using ::derfcdx;

using ::derf_invdx;
using ::derfc_invdx;

}

  UnaryTblEntry<double> UnaryTable_double[] = {
  {"abs",       abs,         "abs(obj)   -- Absolute value"},
  {"exp",       eval64::exp,  "exp(obj)   -- Exponentiation with respect to e"},
  {"log",       eval64::log,  "log(obj)   -- Natural logarithm"},
  {"B",         Bernoulli,    "B(obj)     -- Bernoulli Function"},
  {"dBdx",      derBernoulli, "dBdx(obj)  -- derivative Bernoulli wrt arg"},
  {"step",      step,         "step(obj)  -- step function"},
  {"sgn",       sgn,          "sgn(obj)   -- sign function"},
  {"acosh",     acosh,        "acosh(obj)   -- inverse hyperbolic cosine function"},
  {"asinh",     asinh,        "asinh(obj)   -- inverse hyperbolic sine function"},
  {"atanh",     atanh,        "atanh(obj)   -- inverse hyperbolic tangent function"},
  {"cosh",      cosh,         "cosh(obj)   -- hyperbolic sine function"},
  {"sinh",      sinh,         "sinh(obj)   -- hyperbolic cosine function"},
  {"tanh",      tanh,         "tanh(obj)   -- hyperbolic tangent function"},
  {"erf",       eval64::erf,          "erf(obj)   -- error function"},
  {"erfc",      eval64::erfc,         "erfc(obj)  -- complementary error function"},
  {"erf_inv",       erf_inv,          "erf(obj)   -- inverse error function"},
  {"erfc_inv",      erfc_inv,         "erfc(obj)  -- inverse complementary error function"},
  {"derfdx",    eval64::derfdx,       "derfdx(obj)   -- derivative of error function"},
  {"derfcdx",   eval64::derfcdx,      "derfcdx(obj)  -- derivative of complementary error function"},
  {"derf_invdx",    eval64::derf_invdx,       "derf_invdx(obj)   -- derivative of inverse error function"},
  {"derfc_invdx",   eval64::derfc_invdx,      "derfc_invdx(obj)  -- derivative of inverse complementary error function"},
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
  {"gfi", gfi,             "gfi(obj1, obj2)       -- Gauss Fermi Integral"},
  {"dgfidx", dgfidx,         "dgfidx(obj1, obj2)     -- Gauss Fermi Integral Derivative"},
  {"igfi", igfi,           "igfi(obj1, obj2)      -- Inverse Gauss Fermi Integral"},
  {"digfidx", digfidx,     "digfidx(obj1, obj2)   -- Inverse Gauss Fermi Integral Derivative"},
  {"min",  min,            "min(obj1, obj2)       -- minimum of obj1 and obj2"},
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

#if 0
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
#endif

float128 acosh(float128 x)
{
  return boost::multiprecision::acosh(x);
}

float128 asinh(float128 x)
{
  return boost::multiprecision::asinh(x);
}

float128 atanh(float128 x)
{
  return boost::multiprecision::atanh(x);
}

float128 cosh(float128 x)
{
  return boost::multiprecision::cosh(x);
}

float128 sinh(float128 x)
{
  return boost::multiprecision::sinh(x);
}

float128 tanh(float128 x)
{
  return boost::multiprecision::tanh(x);
}

float128 erf(float128 x)
{
  return boost::multiprecision::erf(x);
}

float128 erfc(float128 x)
{
  return boost::math::erfc(x);
}

using ::derfdx;
using ::derfcdx;
using ::derf_invdx;
using ::derfc_invdx;
}



  UnaryTblEntry<float128> UnaryTable_float128[] = {
  {"abs",      eval128::abs,         "abs(obj)   -- Absolute value"},
  {"exp",      eval128::exp,          "exp(obj)   -- Exponentiation with respect to e"},
  {"log",      eval128::log,          "log(obj)   -- Natural logarithm"},
  {"B",        Bernoulli,    "B(obj)     -- Bernoulli Function"},
  {"dBdx",     derBernoulli, "dBdx(obj)  -- derivative Bernoulli wrt arg"},
  {"step",     step,         "step(obj)  -- step function"},
  {"sgn",      sgn,          "sgn(obj)   -- sign function"},
  {"acosh",    eval128::acosh,        "acosh(obj)   -- inverse hyperbolic cosine function"},
  {"asinh",    eval128::asinh,        "asinh(obj)   -- inverse hyperbolic sine function"},
  {"atanh",    eval128::atanh,        "atanh(obj)   -- inverse hyperbolic tangent function"},
  {"cosh",     eval128::cosh,         "cosh(obj)   -- hyperbolic sine function"},
  {"sinh",     eval128::sinh,         "sinh(obj)   -- hyperbolic cosine function"},
  {"tanh",     eval128::tanh,         "tanh(obj)   -- hyperbolic tangent function"},
  {"erf",      eval128::erf,          "erf(obj)   -- error function"},
  {"erfc",     eval128::erfc,         "erfc(obj)  -- complementary error function"},
  {"erf_inv",      erf_inv,  "erf_inv(obj)   -- error function"},
  {"erfc_inv",     erfc_inv,         "erfc_inv(obj)  -- complementary error function"},
  {"derfdx",   eval128::derfdx,       "derfdx(obj)   -- derivative of error function"},
  {"derfcdx",  eval128::derfcdx,      "derfcdx(obj)  -- derivative of complementary error function"},
  {"derf_invdx",   eval128::derf_invdx,       "derfdx(obj)   -- derivative of inverse error function"},
  {"derfc_invdx",  eval128::derfc_invdx,      "derfcdx(obj)  -- derivative of inverse complementary error function"},
  {"Fermi",    Fermi,        "Fermi(obj)  -- Fermi Integral"},
  {"dFermidx", dFermidx,     "dFermidx(obj)  -- derivative of Fermi Integral"},
  {"InvFermi",     InvFermi,        "InvFermi(obj)  -- inverse of the Fermi Integral"},
  {"dInvFermidx",  dInvFermidx,     "dInvFermidx(obj)  -- derivative of InvFermi Integral"},
  {"!",  logical_not,     "!obj  -- Logical Not"},
  {"vec_sum",   vec_sum, "Vector Summation"},
  {"vec_max",   vec_max, "Vector Maximum"},
  {"vec_min",   vec_min, "Vector Minimum"},
  {nullptr, nullptr, nullptr}
  };

  BinaryTblEntry<float128> BinaryTable_float128[] = {
  {"gfi", gfi,             "gfi(obj1, obj2)       -- Gauss Fermi Integral"},
  {"dgfidx", dgfidx,         "dgfidx(obj1, obj2)     -- Gauss Fermi Integral Derivative"},
  {"igfi", igfi,           "igfi(obj1, obj2)      -- Inverse Gauss Fermi Integral"},
  {"digfidx", digfidx,     "digfidx(obj1, obj2)   -- Inverse Gauss Fermi Integral Derivative"},
  {"min",  min,            "min(obj1, obj2)       -- minimum of obj1 and obj2"},
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
    std::vector<const std::vector<DoubleType> *> vvals(cnt);
    std::vector<DoubleType> result(1);
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

  if (it == tclMathFuncMap_.end())
  {
    dsAssert(it != tclMathFuncMap_.end(), "UNEXPECTED");
    return;
  }

  const size_t tclcount = (it->second).second;

  const size_t cnt = vvals.size();

  std::vector<ObjectHolder> tclObjVector;

  if (tclcount != cnt)
  {
    std::ostringstream os;
    os << "function registered with \"" << tclcount << "\" arguments and \"" << cnt << "\" were provided: \"" << func << "\"";
    error += os.str();
  }
  else
  {
    ObjectHolder procedure = (it->second).first;

    Interpreter MyInterp;
    //// put the objects in here
    tclObjVector.resize(cnt);

    size_t numelems = 0;
    for (size_t i = 0; i < cnt; ++i)
    {
      if (vvals[i] == nullptr)
      {
        tclObjVector[i] = ObjectHolder(static_cast<double>(dvals[i]));
      }
      else
      {
        tclObjVector[i].clear();
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
          tclObjVector[i] = ObjectHolder(static_cast<double>(vvals[i]->operator[](j)));
        }
      }

      bool ok = MyInterp.RunCommand(procedure, tclObjVector);

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
  }
  else
  {
      std::ostringstream os;
      os << "could not find function \"" << func << "\"";
      error += os.str();
  }
}

template <typename DoubleType>
bool MathEval<DoubleType>::AddTclMath(const std::string &funcname, ObjectHolder procedure, size_t numargs, std::string &error)
{
  if (!procedure.IsCallable())
  {
    std::ostringstream os;
    os << "procedure given for \"" << funcname << "\" is not callable";
    error += os.str();
  }
  else
  {
    tclMathFuncMap_[funcname] = std::make_pair(procedure, numargs);
  }
  return error.empty();
}
#if 0
template <typename DoubleType>
void MathEval<DoubleType>::AddTclMath(const std::string &funcname, size_t numargs)
{
  ///Really need to make sure number of arguments make sense in tcl api
  tclMathFuncMap_[funcname] = numargs;
}
#endif

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


