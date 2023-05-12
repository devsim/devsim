/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef LOGICAL_MATH_HH
#define LOGICAL_MATH_HH
template <typename DoubleType>
inline DoubleType ifelsefunc(DoubleType x, DoubleType y, DoubleType z)
{
  return (x) ? y : z;
}

template <typename DoubleType>
inline DoubleType logical_and(DoubleType x, DoubleType y)
{
  return (x && y);
}

template <typename DoubleType>
inline DoubleType logical_or(DoubleType x, DoubleType y)
{
  return (x || y);
}

template <typename DoubleType>
inline DoubleType logical_eq(DoubleType x, DoubleType y)
{
  return (x == y);
}

template <typename DoubleType>
inline DoubleType logical_lt(DoubleType x, DoubleType y)
{
  return (x < y);
}

template <typename DoubleType>
inline DoubleType logical_lte(DoubleType x, DoubleType y)
{
  return (x <= y);
}

template <typename DoubleType>
inline DoubleType logical_gt(DoubleType x, DoubleType y)
{
  return (x > y);
}

template <typename DoubleType>
inline DoubleType logical_gte(DoubleType x, DoubleType y)
{
  return (x >= y);
}

template <typename DoubleType>
inline DoubleType logical_not(DoubleType x)
{
  return !x;
}

#endif
