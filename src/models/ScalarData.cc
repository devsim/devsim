/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "ScalarData.hh"
#include "ParallelOpEqual.hh"

#include "dsAssert.hh"

template <typename T, typename DoubleType>
ScalarData<T, DoubleType>::ScalarData(const T &em) : refdata(0), isuniform(false), uniform_value(0.0)
{
  if (em.IsUniform())
  {
    isuniform     = true;
    uniform_value = em.template GetUniformValue<DoubleType>();
  }
  else
  {
    refdata = &em;
//    values = em.GetScalarValues();
  }

  length = em.GetLength();

}

template <typename T, typename DoubleType>
ScalarData<T, DoubleType>::ScalarData(const std::vector<DoubleType> &esl) : refdata(0), isuniform(false), uniform_value(0.0)
{
  values = esl;
  length = values.size();
}

template <typename T, typename DoubleType>
ScalarData<T, DoubleType>::ScalarData(DoubleType v, size_t l) : refdata(0), isuniform(true), uniform_value(v), length(l)
{
}

template <typename T, typename DoubleType>
ScalarData<T, DoubleType>::ScalarData(const ScalarData<T, DoubleType> &em) : refdata(em.refdata), values(em.values), isuniform(em.isuniform), uniform_value(em.uniform_value), length(em.length)
{
}

template <typename T, typename DoubleType>
ScalarData<T, DoubleType> &ScalarData<T, DoubleType>::operator=(const ScalarData &em)
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

template <typename T, typename DoubleType>
void ScalarData<T, DoubleType>::MakeAssignable() const
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
    values = refdata->template GetScalarValues<DoubleType>();
    refdata = nullptr;
  }
}

template <typename T, typename DoubleType>
DoubleType ScalarData<T, DoubleType>::operator[](size_t x) const
{
  DoubleType ret = 0.0;
  if (isuniform)
  {
    ret = uniform_value;
  }
  else if (refdata)
  {
    const std::vector<DoubleType> &y = refdata->template GetScalarValues<DoubleType>();

    ret = y[x];
  }
  else
  {
    ret = values[x];
  }

  return ret;
}

template <typename T, typename DoubleType>
DoubleType &ScalarData<T, DoubleType>::operator[](size_t x)
{
  MakeAssignable();

  return values[x];
}

template <typename T, typename DoubleType>
const std::vector<DoubleType> &ScalarData<T, DoubleType>::GetScalarList() const
{
  const std::vector<DoubleType> *data = &values;

  if (isuniform)
  {
    //// We are still uniform
    values.clear();
    values.resize(length, uniform_value);
  }
  else if (refdata)
  {
    data = &(refdata->template GetScalarValues<DoubleType>());
  }

  return *data;
}

template <typename T, typename DoubleType> template <typename V>
ScalarData<T, DoubleType> &ScalarData<T, DoubleType>::op_equal_model(const T &nm, const V &myop)
{
  ScalarData<T, DoubleType> onsd(nm);
  this->op_equal_data(onsd, myop);
  return *this;
}

template <typename T, typename DoubleType> template <typename V>
ScalarData<T, DoubleType> &ScalarData<T, DoubleType>::op_equal_data(const ScalarData<T, DoubleType> &esd, const V &myop)
{
  if (isuniform && esd.isuniform)
  {
    myop(uniform_value, esd.uniform_value);
  }
  else if (esd.isuniform)
  {
    const DoubleType &oval = esd.uniform_value;
    this->op_equal_scalar(oval, myop);
  }
  else
  {
    MakeAssignable();

    const std::vector<DoubleType> &ovals = esd.GetScalarList();

    SerialVectorVectorOpEqual<V, DoubleType> foo(values, ovals, myop);
    OpEqualRun(foo, values.size());
  }
  return *this;
}

template <typename T, typename DoubleType> template <typename V>
ScalarData<T, DoubleType> &ScalarData<T, DoubleType>::op_equal_scalar(const DoubleType &v, const V &myop)
{
  if (isuniform)
  {
    myop(uniform_value, v);
  }
  else
  {
    MakeAssignable();
    SerialVectorScalarOpEqual<V, DoubleType> foo(values, v, myop);
    OpEqualRun(foo, values.size());
  }
  return *this;
}


template <typename T, typename DoubleType>
ScalarData<T, DoubleType> &ScalarData<T, DoubleType>::times_equal_model(const T &nm)
{
  if (this->IsZero() || nm.IsOne())
  {
  }
  else if ((nm.IsZero()))
  {
    *this = ScalarData<T, DoubleType>(0.0, length);
  }
  else if (this->IsOne())
  {
    *this = ScalarData<T, DoubleType>(nm);
  }
  else
  {
    this->op_equal_model(nm, ScalarDataHelper::times_equal<DoubleType>());
  }

  return *this;
}

template <typename T, typename DoubleType>
ScalarData<T, DoubleType> &ScalarData<T, DoubleType>::times_equal_data(const ScalarData<T, DoubleType> &esd)
{
  if (this->IsZero() || esd.IsOne())
  {
  }
  else if ((esd.IsZero()))
  {
    *this = ScalarData<T, DoubleType>(0.0, length);
  }
  else if (this->IsOne())
  {
    *this = esd;
  }
  else
  {
    this->op_equal_data(esd, ScalarDataHelper::times_equal<DoubleType>());
  }

  return *this;
}

template <typename T, typename DoubleType>
ScalarData<T, DoubleType> &ScalarData<T, DoubleType>::times_equal_scalar(DoubleType x)
{
  if ((this->IsZero()) || (x == 1.0))
  {
    // we are already 0.0
    // or
    // unit factor
  }
  else if (x == 0.0)
  {
    *this = ScalarData<T, DoubleType>(0.0, length);
  }
  else if (this->IsOne())
  {
    *this = ScalarData<T, DoubleType>(x, length);
  }
  else
  {
    this->op_equal_scalar(x, ScalarDataHelper::times_equal<DoubleType>());
  }

  return *this;
}


template <typename T, typename DoubleType>
ScalarData<T, DoubleType> &ScalarData<T, DoubleType>::plus_equal_model(const T &nm)
{
  if (this->IsZero())
  {
    *this = ScalarData<T, DoubleType>(nm);
  }
  else
  {
    op_equal_model(nm, ScalarDataHelper::plus_equal<DoubleType>());
  }

  return *this;
}

template <typename T, typename DoubleType>
ScalarData<T, DoubleType> &ScalarData<T, DoubleType>::plus_equal_data(const ScalarData<T, DoubleType> &esd)
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
    this->op_equal_data(esd, ScalarDataHelper::plus_equal<DoubleType>());
  }

  return *this;
}


template <typename T, typename DoubleType>
ScalarData<T, DoubleType> &ScalarData<T, DoubleType>::plus_equal_scalar(DoubleType x)
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
    this->op_equal_scalar(x, ScalarDataHelper::plus_equal<DoubleType>());
  }

  return *this;
}


