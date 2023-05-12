/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef MISC_MATH_FUNC_HH
#define MISC_MATH_FUNC_HH
template <typename DoubleType>
DoubleType erfc_inv(DoubleType);

template <typename DoubleType>
DoubleType erf_inv(DoubleType);

template <typename DoubleType>
DoubleType derfdx(DoubleType);

template <typename DoubleType>
DoubleType derfcdx(DoubleType);

template <typename DoubleType>
DoubleType derf_invdx(DoubleType);

template <typename DoubleType>
DoubleType derfc_invdx(DoubleType);

template <typename DoubleType>
DoubleType vec_sum(DoubleType);

template <typename DoubleType>
DoubleType vec_max(DoubleType);

template <typename DoubleType>
DoubleType vec_min(DoubleType);

template <typename DoubleType>
DoubleType dot2dfunc(DoubleType, DoubleType, DoubleType, DoubleType);
#endif

