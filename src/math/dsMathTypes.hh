/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef DS_MATH_TYPES_HH
#define DS_MATH_TYPES_HH
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
#endif
#include <vector>
#include <complex>
namespace dsMath
{

template <typename T>
struct ComplexTypeWrapper
{
};

template <>
struct ComplexTypeWrapper<double>
{
    using type = std::complex<double>;
};

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
struct ComplexTypeWrapper<float128>
{
    using type = complex128;
};
#endif

template <typename DoubleType>
using ComplexDouble_t = typename ComplexTypeWrapper<DoubleType>::type;

template <typename DoubleType>
using ComplexDoubleVec_t = std::vector<ComplexDouble_t<DoubleType>>;

template <typename DoubleType>
using DoubleVec_t = std::vector<DoubleType>;

typedef std::vector<int>    IntVec_t;

}
#endif
