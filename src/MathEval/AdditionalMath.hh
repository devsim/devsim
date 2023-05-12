/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef ADDITIONAL_MATH_HH
#define ADDITIONAL_MATH_HH

template <typename U>
inline U min(U x, U y)
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

template <typename U>
inline U max(U x, U y)
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

template <typename U>
inline U step(U x)
{
    return (x >= 0.0) ? 1.0 : 0.0;
}

template <typename U>
inline U sgn(U x)
{
    return (x >= 0.0) ? 1.0 : -1.0;
}

#endif

