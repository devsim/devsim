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

#include "kahan.hh"
#include <cmath>
using std::abs;

template <typename DoubleType>
kahan<DoubleType> &kahan<DoubleType>::operator+=(DoubleType v)
{
    if (abs(value_) < abs(correction_))
    {
      const DoubleType t = value_;
      value_ = correction_;
      correction_ = t;
    }

    const DoubleType x = value_ + v;
    DoubleType c = x;
    if (abs(value_) < abs(v))
    {
      c -= v;
      c -= value_;
    }
    else
    {
      c -= value_;
      c -= v;
    }
    value_      = x;
    correction_ -= c;
    return *this;
}

template <typename DoubleType>
kahan<DoubleType> &kahan<DoubleType>::operator-=(DoubleType v)
{
    (*this) += (-v);
    return *this;
}

template class kahan<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class kahan<float128>;
#endif


