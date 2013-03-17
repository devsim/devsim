/***
DEVSIM
Copyright 2013 Devsim LLC

This file is part of DEVSIM.

DEVSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, version 3.

DEVSIM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with DEVSIM.  If not, see <http://www.gnu.org/licenses/>.
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
