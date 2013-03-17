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

#ifndef DS_MATH_TYPES_HH
#define DS_MATH_TYPES_HH
#include <vector>
#include <complex>
namespace dsMath
{

typedef std::complex<double>         ComplexDouble_t;
typedef std::vector<ComplexDouble_t> ComplexDoubleVec_t;

typedef std::vector<double> DoubleVec_t;
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
