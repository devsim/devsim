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
