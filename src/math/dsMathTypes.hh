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
