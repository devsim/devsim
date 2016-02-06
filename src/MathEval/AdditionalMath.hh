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

#ifndef ADDITIONAL_MATH_HH
#define ADDITIONAL_MATH_HH

inline double min(double x, double y)
{
  if (x <= y)
  {
    return x;
  }
  else
  {
    return y;
  }
}

inline double max(double x, double y)
{
  if (x >= y)
  {
    return x;
  }
  else
  {
    return y;
  }
}

inline double step(double x)
{
    return (x >= 0.0) ? 1.0 : 0.0;
}

inline double sgn(double x)
{
    return (x >= 0.0) ? 1.0 : -1.0;
}

#if 0
inline double parallel(double x, double y)
{
  return ((x * y)/(x + y));
}

inline double dparallel_dx(double x, double y)
{
  double ret = y/(x + y);
  ret *= ret;
  return ret;
}
#endif

#endif
