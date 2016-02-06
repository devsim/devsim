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

#include "ScalarData.hh"
#include "ParallelOpEqual.hh"

#include "dsAssert.hh"

template <typename T>
ScalarData<T>::ScalarData(const T &em) : refdata(0), isuniform(false), uniform_value(0.0)
{
  if (em.IsUniform())
  {
    isuniform     = true;
    uniform_value = em.GetUniformValue();
  }
  else
  {
    refdata = &em;
//    values = em.GetScalarValues();
  }

  length = em.GetLength();

}

template <typename T>
ScalarData<T>::ScalarData(const ScalarList &esl) : refdata(0), isuniform(false), uniform_value(0.0)
{
  values = esl;
  length = values.size();
}

template <typename T>
ScalarData<T>::ScalarData(double v, size_t l) : refdata(0), isuniform(true), uniform_value(v), length(l)
{
}

template <typename T>
ScalarData<T>::ScalarData(const ScalarData<T> &em) : refdata(em.refdata), values(em.values), isuniform(em.isuniform), uniform_value(em.uniform_value), length(em.length)
{
}

template <typename T>
ScalarData<T> &ScalarData<T>::operator=(const ScalarData &em)
{
  if (this != &em)
  {
    refdata       = em.refdata;
    values        = em.values;
    isuniform     = em.isuniform;
    uniform_value = em.uniform_value;
    length        = em.length;
  }
  return *this;
}

template <typename T>
ScalarData<T>::ScalarData() : refdata(0), isuniform(true), uniform_value(0.0), length(0)
{
}

template <typename T>
void ScalarData<T>::MakeAssignable() const
{
  if (isuniform)
  {
    values.clear();
    values.resize(length, uniform_value);
    uniform_value = 0.0;
    isuniform = false;
  }
  else if (refdata)
  {
    values = refdata->GetScalarValues();
    refdata = NULL;
  }
}

template <typename T>
double ScalarData<T>::operator[](ScalarList::size_type x) const
{
  double ret = 0.0;
  if (isuniform)
  {
    ret = uniform_value;
  }
  else if (refdata)
  {
    const ScalarList &y = refdata->GetScalarValues();

    ret = y[x];
  }
  else
  {
    ret = values[x];
  }

  return ret;
}

template <typename T>
double &ScalarData<T>::operator[](ScalarList::size_type x)
{
  MakeAssignable();

  return values[x];
}

template <typename T>
const ScalarList &ScalarData<T>::GetScalarList() const
{
  const ScalarList *data = &values;

  if (isuniform)
  {
    //// We are still uniform
    values.clear();
    values.resize(length, uniform_value);
  }
  else if (refdata)
  {
    data = &(refdata->GetScalarValues());
  }

  return *data;
}

template <typename T> template <typename U>
ScalarData<T> &ScalarData<T>::op_equal(const T &nm, const U &myop)
{
  ScalarData<T> onsd(nm);
  this->op_equal(onsd, myop);
  return *this;
}

template <typename T> template <typename U>
ScalarData<T> &ScalarData<T>::op_equal(const ScalarData<T> &esd, const U &myop)
{
  if (isuniform && esd.isuniform)
  {
    myop(uniform_value, esd.uniform_value);
  }
  else if (esd.isuniform)
  {
    const double &oval = esd.uniform_value;
    this->op_equal(oval, myop);
  }
  else
  {
    MakeAssignable();

    const ScalarList &ovals = esd.GetScalarList();

    SerialVectorVectorOpEqual<U> foo(values, ovals, myop);
    OpEqualRun(foo, values.size());
  }
  return *this;
}

template <typename T> template <typename U>
ScalarData<T> &ScalarData<T>::op_equal(const double &v, const U &myop)
{
  if (isuniform)
  {
    myop(uniform_value, v);
  }
  else
  {
    MakeAssignable();
    SerialVectorScalarOpEqual<U> foo(values, v, myop);
    OpEqualRun(foo, values.size());
  }
  return *this;
}


template <typename T>
ScalarData<T> &ScalarData<T>::operator*=(const T &nm)
{
  if (this->IsZero() || nm.IsOne())
  {
  }
  else if ((nm.IsZero()))
  {
    *this = ScalarData<T>(0.0, length);
  }
  else if (this->IsOne())
  {
    *this = ScalarData<T>(nm);
  }
  else
  {
    this->op_equal(nm, ScalarDataHelper::times_equal());
  }

  return *this;
}

template <typename T>
ScalarData<T> &ScalarData<T>::operator*=(const ScalarData<T> &esd)
{
  if (this->IsZero() || esd.IsOne())
  {
  }
  else if ((esd.IsZero()))
  {
    *this = ScalarData<T>(0.0, length);
  }
  else if (this->IsOne())
  {
    *this = esd;
  }
  else
  {
    this->op_equal(esd, ScalarDataHelper::times_equal());
  }

  return *this;
}

template <typename T>
ScalarData<T> &ScalarData<T>::operator*=(double x)
{
  if ((this->IsZero()) || (x == 1.0))
  {
    // we are already 0.0
    // or
    // unit factor
  }
  else if (x == 0.0)
  {
    *this = ScalarData<T>(0.0, length);
  }
  else if (this->IsOne())
  {
    *this = ScalarData<T>(x, length);
  }
  else
  {
    this->op_equal(x, ScalarDataHelper::times_equal());
  }

  return *this;
}


template <typename T>
ScalarData<T> &ScalarData<T>::operator+=(const T &nm)
{
  if (this->IsZero())
  {
    *this = ScalarData<T>(nm);
  }
  else
  {
    op_equal(nm, ScalarDataHelper::plus_equal());
  }

  return *this;
}

template <typename T>
ScalarData<T> &ScalarData<T>::operator+=(const ScalarData<T> &esd)
{
  if (this->IsZero())
  {
    *this = esd;
  }
  else if (esd.IsZero())
  {
  }
  else
  {
    this->op_equal(esd, ScalarDataHelper::plus_equal());
  }

  return *this;
}


template <typename T>
ScalarData<T> &ScalarData<T>::operator+=(double x)
{
  if (x == 0.0)
  {
    // do nothing
  }
  else if (this->IsZero())
  {
    //// IsZero() implies isuniform
    uniform_value = x;
  }
  else
  {
    this->op_equal(x, ScalarDataHelper::plus_equal());
  }

  return *this;
}


