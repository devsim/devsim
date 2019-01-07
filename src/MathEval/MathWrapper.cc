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

#include "MathWrapper.hh"

#include "dsAssert.hh"

#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
using boost::multiprecision::pow;
#endif

#include <sstream>
#include <cmath>
#include <limits>

namespace Eqomfp {

template <typename DoubleType>
MathWrapper<DoubleType>::~MathWrapper()
{
}

template <typename DoubleType>
void MathWrapper<DoubleType>::Evaluate(const std::vector<DoubleType> &dvals, const std::vector<const std::vector<DoubleType> *> &vvals, std::string &error, std::vector<DoubleType> &result, const size_t vbeg, const size_t vend) const
{
  if (dvals.size() != this->GetNumberArguments())
  {
    std::ostringstream os;
    os << "Function " << name_ << " available with " << this->GetNumberArguments() << " but called with " << dvals.size();
    error += os.str();
  }
  else
  {
    this->DerivedEvaluate(dvals, vvals, result, vbeg, vend);
  }
}

template <typename DoubleType>
DoubleType MathWrapper<DoubleType>::Evaluate(const std::vector<DoubleType> &vals, std::string &error) const
{
  DoubleType x = 0.0;

  if (vals.size() != this->GetNumberArguments())
  {
    std::ostringstream os;
    os << "Function " << name_ << " available with " << this->GetNumberArguments() << " but called with " << vals.size();
    error += os.str();
  }
  else
  {
    x = this->DerivedEvaluate(vals);
  }

  return x;
}

template <typename DoubleType>
void MathWrapper1<DoubleType>::DerivedEvaluate(const std::vector<DoubleType> &/*dvals*/, const std::vector<const std::vector<DoubleType> *> &vvals, std::vector<DoubleType> &result, const size_t vbeg, const size_t vend) const
{
  dsAssert(vvals[0] != nullptr, "UNEXPECTED");

//  const std::vector<DoubleType> &vals = *vvals[0];
  DoubleType       *vr = &result[vbeg];
  const DoubleType *v0 = &((*vvals[0])[vbeg]);

//    std::transform(vals.begin(), vals.end(), result.begin(), std::ptr_fun(funcptr_));
  for (size_t i = vbeg; i < vend; ++i)
  {
    *(vr++) = funcptr_(*(v0++));
  }
}

template <typename DoubleType>
DoubleType MathWrapper1<DoubleType>::DerivedEvaluate(const std::vector<DoubleType> &vals) const
{
  return funcptr_(vals[0]);
}

template <typename DoubleType>
void MathWrapper2<DoubleType>::DerivedEvaluate(const std::vector<DoubleType> &dvals, const std::vector<const std::vector<DoubleType> *> &vvals, std::vector<DoubleType> &result, const size_t vbeg, const size_t vend) const
{
  dsAssert(vvals[0] || vvals[1], "UNEXPECTED");

  if (vvals[0] && vvals[1])
  {
    const DoubleType *v0 = &((*vvals[0])[vbeg]);
    const DoubleType *v1 = &((*vvals[1])[vbeg]);
    DoubleType       *vr = &result[vbeg];

    for (size_t i = vbeg; i < vend; ++i)
    {
      *(vr++) = funcptr_(*(v0++), *(v1++));
    }

  }
  else if (vvals[0])
  {
    const DoubleType *v0 = &((*vvals[0])[vbeg]);
    const DoubleType dval1 = dvals[1];
    DoubleType       *vr = &result[vbeg];

    for (size_t i = vbeg; i < vend; ++i)
    {
      *(vr++) = funcptr_(*(v0++), dval1);
    }
  }
  else if (vvals[1])
  {
    const DoubleType *v1 = &((*vvals[1])[vbeg]);
    const DoubleType dval0 = dvals[0];
    DoubleType       *vr = &result[vbeg];

    for (size_t i = vbeg; i < vend; ++i)
    {
      *(vr++) = funcptr_(dval0, *(v1++));
    }
  }
}

template <typename DoubleType>
DoubleType MathWrapper2<DoubleType>::DerivedEvaluate(const std::vector<DoubleType> &vals) const
{
  return funcptr_(vals[0], vals[1]);
}

template <typename DoubleType>
void MathWrapper3<DoubleType>::DerivedEvaluate(const std::vector<DoubleType> &dvals, const std::vector<const std::vector<DoubleType> *> &vvals, std::vector<DoubleType> &result, const size_t vbeg, const size_t vend) const
{
  dsAssert(vvals[0] || vvals[1] || vvals[2], "UNEXPECTED");

  DoubleType vals[3];
  for (size_t i = 0; i < 3; ++i)
  {
    vals[i] = dvals[i];
  }

  for (size_t i = vbeg; i < vend; ++i)
  {
    for (size_t j = 0; j < 3; ++j)
    {
      if (vvals[j])
      {
        vals[j] = (*vvals[j])[i];
      }
    }

    result[i] =  (*funcptr_)(vals[0], vals[1], vals[2]);
  }
}

template <typename DoubleType>
DoubleType MathWrapper3<DoubleType>::DerivedEvaluate(const std::vector<DoubleType> &vals) const
{
  return funcptr_(vals[0], vals[1], vals[2]);
}


template <typename DoubleType>
void MathWrapper4<DoubleType>::DerivedEvaluate(const std::vector<DoubleType> &dvals, const std::vector<const std::vector<DoubleType> *> &vvals, std::vector<DoubleType> &result, const size_t vbeg, const size_t vend) const
{
  dsAssert(vvals[0] || vvals[1] || vvals[2] || vvals[3], "UNEXPECTED");

  DoubleType vals[4];
  for (size_t i = 0; i < 4; ++i)
  {
    vals[i] = dvals[i];
  }

  for (size_t i = vbeg; i < vend; ++i)
  {
    for (size_t j = 0; j < 4; ++j)
    {
      if (vvals[j])
      {
        vals[j] = (*vvals[j])[i];
      }
    }

    result[i] =  (*funcptr_)(vals[0], vals[1], vals[2], vals[3]);
  }
}

template <typename DoubleType>
DoubleType MathWrapper4<DoubleType>::DerivedEvaluate(const std::vector<DoubleType> &vals) const
{
  return funcptr_(vals[0], vals[1], vals[2], vals[3]);
}

namespace
{
template <typename DoubleType>
inline bool IsInt(DoubleType x)
{
  double y;
  return ((modf(static_cast<double>(x), &y) == 0) &&
          (x >= std::numeric_limits<int>::min()) &&
          (x <= std::numeric_limits<int>::max())
         );
}
}


template <typename DoubleType>
void PowWrapper<DoubleType>::DerivedEvaluate(const std::vector<DoubleType> &dvals, const std::vector<const std::vector<DoubleType> *> &vvals, std::vector<DoubleType> &result, const size_t vbeg, const size_t vend) const
{
  dsAssert(vvals[0] || vvals[1], "UNEXPECTED");

  if (vvals[0] && vvals[1])
  {
    const DoubleType *v0 = &((*vvals[0])[vbeg]);
    const DoubleType *v1 = &((*vvals[1])[vbeg]);
    DoubleType       *vr = &result[vbeg];

    for (size_t i = vbeg; i < vend; ++i)
    {
      *(vr++) = pow(*(v0++), *(v1++));
    }

  }
  else if (vvals[0])
  {
    const DoubleType *v0 = &((*vvals[0])[vbeg]);
    const DoubleType dval1 = dvals[1];
    DoubleType       *vr = &result[vbeg];

    if (IsInt(dval1))
    {
      for (size_t i = vbeg; i < vend; ++i)
      {
        *(vr++) = pow(*(v0++), static_cast<int>(dval1));
      }
    }
    else
    {
      for (size_t i = vbeg; i < vend; ++i)
      {
        *(vr++) = pow(*(v0++), dval1);
      }
    }
  }
  else if (vvals[1])
  {
    const DoubleType *v1 = &((*vvals[1])[vbeg]);
    const DoubleType dval0 = dvals[0];
    DoubleType       *vr = &result[vbeg];

    for (size_t i = vbeg; i < vend; ++i)
    {
      *(vr++) = pow(dval0, *(v1++));
    }
  }
}

template <typename DoubleType>
DoubleType PowWrapper<DoubleType>::DerivedEvaluate(const std::vector<DoubleType> &vals) const
{
  const DoubleType e = vals[1];
  if (IsInt(e))
  {
    return pow(vals[0], static_cast<int>(e));
  }
  else
  {
    return pow(vals[0], e);
  }
}

template class MathWrapper<double>;
template class MathWrapper1<double>;
template class MathWrapper2<double>;
template class MathWrapper3<double>;
template class MathWrapper4<double>;
template class PowWrapper<double>;

#ifdef DEVSIM_EXTENDED_PRECISION
template class MathWrapper<float128>;
template class MathWrapper1<float128>;
template class MathWrapper2<float128>;
template class MathWrapper3<float128>;
template class MathWrapper4<float128>;
template class PowWrapper<float128>;
#endif

}///end Eqomfp

