/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef DS_MATH_TYPES_HH
#define DS_MATH_TYPES_HH
#include <vector>
#include <complex>
namespace dsMath
{

template <typename DoubleType>
using ComplexDouble_t = std::complex<DoubleType>;

template <typename DoubleType>
using ComplexDoubleVec_t = std::vector<std::complex<DoubleType>>;

template <typename DoubleType>
using DoubleVec_t = std::vector<DoubleType>;

typedef std::vector<int>    IntVec_t;

template <typename T, class U >
void CopySTLVectorToRaw(T *nv, const U &ov)
{
    T *p = nv;
    typename U::const_iterator it = ov.begin();
    for (it = ov.begin(); it != ov.end(); ++it)
    {
#if 0
        *(p++) = *it;
#endif
        *(p++) = *(reinterpret_cast<const T *>(&(*it)));
    }
}

//// Destination must be correct size
template <typename T, class U >
void CopyRawToSTLVector(T *nv, U &ov)
{
    T *p = nv;
    typename U::iterator it = ov.begin();
    for (it = ov.begin(); it != ov.end(); ++it)
    {
#if 0
        *it = *(p++);
#endif
        *(reinterpret_cast<T *>(&(*it))) = *(p++);
    }
}
}
#endif
