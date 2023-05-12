/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "MiscMathFunc.hh"
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
#endif
#include "BoostConstants.hh"
#include "BoostSpecialFunctions.hh"
using my_policy::use_errno;
#include <cmath>

template <typename DoubleType>
DoubleType derfdx(DoubleType x)
{
#if 1
  static const DoubleType two_div_root_pi = 2.0*boost::math::constants::one_div_root_pi<DoubleType>();
#else
#ifdef DEVSIM_EXTENDED_PRECISION
  static const DoubleType two_div_root_pi = 2.0*boost::math::constants::one_div_root_pi<DoubleType>();
#else
  static const DoubleType two_div_root_pi = M_2_SQRTPI;
#endif
#endif
  return two_div_root_pi*exp(-x*x);
}

template <typename DoubleType>
DoubleType derfcdx(DoubleType x)
{
#if 1
  static const DoubleType mtwo_div_root_pi = -2.0*boost::math::constants::one_div_root_pi<DoubleType>();
#else
#ifdef DEVSIM_EXTENDED_PRECISION
  static const DoubleType mtwo_div_root_pi = -2.0*boost::math::constants::one_div_root_pi<DoubleType>();
#else
  static const DoubleType mtwo_div_root_pi = -M_2_SQRTPI;
#endif
#endif
  return mtwo_div_root_pi*exp(-x*x);
}

template <typename DoubleType>
DoubleType derf_invdx(DoubleType x)
{
  // g'(x) = 1./(f'(g(x)))
  return 1.0 / derfdx(boost::math::erf_inv(x, use_errno()));
}

template <typename DoubleType>
DoubleType derfc_invdx(DoubleType x)
{
  // g'(x) = 1./(f'(g(x)))
  return 1.0 / derfcdx(boost::math::erfc_inv(x, use_errno()));
}


template <typename DoubleType>
DoubleType vec_sum(DoubleType x)
{
  return x;
}

template <typename DoubleType>
DoubleType vec_max(DoubleType x)
{
  return x;
}

template <typename DoubleType>
DoubleType vec_min(DoubleType x)
{
  return x;
}

template <typename DoubleType>
DoubleType dot2dfunc(DoubleType ax, DoubleType ay, DoubleType bx, DoubleType by)
{
  return (ax*bx + ay*by);
}

template <typename DoubleType>
DoubleType erf_inv(DoubleType x)
{
  return boost::math::erf_inv(x, use_errno());
}

template <typename DoubleType>
DoubleType erfc_inv(DoubleType x)
{
  return boost::math::erfc_inv(x, use_errno());
}

template double erf_inv(double x);
template double erfc_inv(double x);
template double derfdx(double x);
template double derfcdx(double x);
template double derf_invdx(double x);
template double derfc_invdx(double x);
template double vec_sum(double x);
template double vec_max(double x);
template double vec_min(double x);
template double dot2dfunc(double ax, double ay, double bx, double by);

#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template float128 erf_inv(float128 x);
template float128 erfc_inv(float128 x);
template float128 derfdx(float128 x);
template float128 derfcdx(float128 x);
template float128 derf_invdx(float128 x);
template float128 derfc_invdx(float128 x);
template float128 vec_sum(float128 x);
template float128 vec_max(float128 x);
template float128 vec_min(float128 x);
template float128 dot2dfunc(float128 ax, float128 ay, float128 bx, float128 by);
#endif

