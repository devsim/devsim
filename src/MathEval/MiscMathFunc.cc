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
#include <cmath>

template <typename DoubleType>
DoubleType derfdx(DoubleType x)
{
#ifndef _WIN32
#warning "Use proper literal"
#endif
    return M_2_SQRTPI*exp(-x*x);
}

template <typename DoubleType>
DoubleType derfcdx(DoubleType x)
{
#ifndef _WIN32
#warning "Use proper literal"
#endif
    return -M_2_SQRTPI*exp(-x*x);
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

