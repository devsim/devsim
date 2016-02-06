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
#include <sstream>
#include <cmath>
#include <limits>

namespace Eqomfp {

MathWrapper::~MathWrapper()
{
}

void MathWrapper::Evaluate(const std::vector<double> &dvals, const std::vector<const std::vector<double> *> &vvals, std::string &error, std::vector<double> &result, const size_t vbeg, const size_t vend) const
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

double MathWrapper::Evaluate(const std::vector<double> &vals, std::string &error) const
{
  double x = 0.0;

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

void MathWrapper1::DerivedEvaluate(const std::vector<double> &/*dvals*/, const std::vector<const std::vector<double> *> &vvals, std::vector<double> &result, const size_t vbeg, const size_t vend) const
{
  dsAssert(vvals[0] != NULL, "UNEXPECTED");

//  const std::vector<double> &vals = *vvals[0];
  double       *vr = &result[vbeg];
  const double *v0 = &((*vvals[0])[vbeg]);

//    std::transform(vals.begin(), vals.end(), result.begin(), std::ptr_fun(funcptr_));
  for (size_t i = vbeg; i < vend; ++i)
  {
    *(vr++) = funcptr_(*(v0++));
  }
}

double MathWrapper1::DerivedEvaluate(const std::vector<double> &vals) const
{
  return funcptr_(vals[0]);
}

void MathWrapper2::DerivedEvaluate(const std::vector<double> &dvals, const std::vector<const std::vector<double> *> &vvals, std::vector<double> &result, const size_t vbeg, const size_t vend) const
{
  dsAssert(vvals[0] || vvals[1], "UNEXPECTED");

  if (vvals[0] && vvals[1])
  {
    const double *v0 = &((*vvals[0])[vbeg]);
    const double *v1 = &((*vvals[1])[vbeg]);
    double       *vr = &result[vbeg];

    for (size_t i = vbeg; i < vend; ++i)
    {
      *(vr++) = funcptr_(*(v0++), *(v1++));
    }

  }
  else if (vvals[0])
  {
    const double *v0 = &((*vvals[0])[vbeg]);
    const double dval1 = dvals[1];
    double       *vr = &result[vbeg];

    for (size_t i = vbeg; i < vend; ++i)
    {
      *(vr++) = funcptr_(*(v0++), dval1);
    }
  }
  else if (vvals[1])
  {
    const double *v1 = &((*vvals[1])[vbeg]);
    const double dval0 = dvals[0];
    double       *vr = &result[vbeg];

    for (size_t i = vbeg; i < vend; ++i)
    {
      *(vr++) = funcptr_(dval0, *(v1++));
    }
  }
}

double MathWrapper2::DerivedEvaluate(const std::vector<double> &vals) const
{
  return funcptr_(vals[0], vals[1]);
}

void MathWrapper3::DerivedEvaluate(const std::vector<double> &dvals, const std::vector<const std::vector<double> *> &vvals, std::vector<double> &result, const size_t vbeg, const size_t vend) const
{
  dsAssert(vvals[0] || vvals[1] || vvals[2], "UNEXPECTED");

  double vals[3];
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

double MathWrapper3::DerivedEvaluate(const std::vector<double> &vals) const
{
  return funcptr_(vals[0], vals[1], vals[2]);
}


void MathWrapper4::DerivedEvaluate(const std::vector<double> &dvals, const std::vector<const std::vector<double> *> &vvals, std::vector<double> &result, const size_t vbeg, const size_t vend) const
{
  dsAssert(vvals[0] || vvals[1] || vvals[2] || vvals[3], "UNEXPECTED");

  double vals[4];
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

double MathWrapper4::DerivedEvaluate(const std::vector<double> &vals) const
{
  return funcptr_(vals[0], vals[1], vals[2], vals[3]);
}

namespace
{
inline bool IsInt(double x)
{
  double y;
  return ((modf(x, &y) == 0) &&
          (x >= std::numeric_limits<int>::min()) &&
          (x <= std::numeric_limits<int>::max())
         );
}
}


void PowWrapper::DerivedEvaluate(const std::vector<double> &dvals, const std::vector<const std::vector<double> *> &vvals, std::vector<double> &result, const size_t vbeg, const size_t vend) const
{
  dsAssert(vvals[0] || vvals[1], "UNEXPECTED");

  if (vvals[0] && vvals[1])
  {
    const double *v0 = &((*vvals[0])[vbeg]);
    const double *v1 = &((*vvals[1])[vbeg]);
    double       *vr = &result[vbeg];

    for (size_t i = vbeg; i < vend; ++i)
    {
      *(vr++) = std::pow(*(v0++), *(v1++));
    }

  }
  else if (vvals[0])
  {
    const double *v0 = &((*vvals[0])[vbeg]);
    const double dval1 = dvals[1];
    double       *vr = &result[vbeg];

    if (IsInt(dval1))
    {
      for (size_t i = vbeg; i < vend; ++i)
      {
        *(vr++) = std::pow(*(v0++), static_cast<int>(dval1));
      }
    }
    else
    {
      for (size_t i = vbeg; i < vend; ++i)
      {
        *(vr++) = std::pow(*(v0++), dval1);
      }
    }
  }
  else if (vvals[1])
  {
    const double *v1 = &((*vvals[1])[vbeg]);
    const double dval0 = dvals[0];
    double       *vr = &result[vbeg];

    for (size_t i = vbeg; i < vend; ++i)
    {
      *(vr++) = std::pow(dval0, *(v1++));
    }
  }
}

double PowWrapper::DerivedEvaluate(const std::vector<double> &vals) const
{
  const double e = vals[1];
  if (IsInt(e))
  {
    return std::pow(vals[0], static_cast<int>(e));
  }
  else
  {
    return std::pow(vals[0], e);
  }
}


}///end Eqomfp

