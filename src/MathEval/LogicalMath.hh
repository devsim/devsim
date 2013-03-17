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

#ifndef LOGICAL_MATH_HH
#define LOGICAL_MATH_HH
inline double ifelsefunc(double x, double y, double z)
{
  return (x) ? y : z;
}

inline double logical_and(double x, double y)
{
  return (x && y);
}

inline double logical_or(double x, double y)
{
  return (x || y);
}

inline double logical_eq(double x, double y)
{
  return (x == y);
}

inline double logical_lt(double x, double y)
{
  return (x < y);
}

inline double logical_lte(double x, double y)
{
  return (x <= y);
}

inline double logical_gt(double x, double y)
{
  return (x > y);
}

inline double logical_gte(double x, double y)
{
  return (x >= y);
}

inline double logical_not(double x)
{
  return !x;
}

#endif
