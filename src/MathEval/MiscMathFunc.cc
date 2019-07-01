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

#include "MiscMathFunc.hh"
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
#endif
#include <cmath>

template <typename DoubleType>
DoubleType derfdx(DoubleType x)
{
#ifdef DEVSIM_EXTENDED_PRECISION
  static const DoubleType two_div_root_pi = 2.0*boost::math::constants::one_div_root_pi<DoubleType>();
#else
  static const DoubleType two_div_root_pi = M_2_SQRTPI;
#endif
  return two_div_root_pi*exp(-x*x);
}

template <typename DoubleType>
DoubleType derfcdx(DoubleType x)
{
#ifdef DEVSIM_EXTENDED_PRECISION
  static const DoubleType mtwo_div_root_pi = -2.0*boost::math::constants::one_div_root_pi<DoubleType>();
#else
  static const DoubleType mtwo_div_root_pi = -M_2_SQRTPI;
#endif
  return mtwo_div_root_pi*exp(-x*x);
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

template double derfdx(double x);
template double derfcdx(double x);
template double vec_sum(double x);
template double vec_max(double x);
template double vec_min(double x);
template double dot2dfunc(double ax, double ay, double bx, double by);

#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template float128 derfdx(float128 x);
template float128 derfcdx(float128 x);
template float128 vec_sum(float128 x);
template float128 vec_max(float128 x);
template float128 vec_min(float128 x);
template float128 dot2dfunc(float128 ax, float128 ay, float128 bx, float128 by);
#endif

