/***
DEVSIM
Copyright 2017 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "ModelDataHolder.hh"

void ModelDataHolder::clear_type(MDtype t) const
{
  if (t == MDtype::DOUBLE)
  {
    std::vector<double>().swap(double_values);
  }
#ifdef DEVSIM_EXTENDED_PRECISION
  else if (t == MDtype::EXTENDED)
  {
    std::vector<float128>().swap(float128_values);
  }
#endif
}

void ModelDataHolder::set_type(MDtype t) const
{
  if (type == t)
  {
    return;
  }
  else if (is_uniform)
  {
    type = t;
  }
#ifdef DEVSIM_EXTENDED_PRECISION
  else if (t == MDtype::EXTENDED)
  {
    float128_values.resize(double_values.size());
    for (size_t i = 0; i < double_values.size(); ++i)
    {
      float128_values[i] = double_values[i];
    }
    type = t;
    clear_type(MDtype::DOUBLE);
  }
  else if (t == MDtype::DOUBLE)
  {
    double_values.resize(float128_values.size());
    for (size_t i = 0; i < float128_values.size(); ++i)
    {
      double_values[i] = static_cast<double>(float128_values[i]);
    }
    type = t;
    clear_type(MDtype::EXTENDED);
  }
#endif
}

bool ModelDataHolder::IsUniform() const
{
  return is_uniform;
}

void ModelDataHolder::expand_uniform() const
{
  if (!is_uniform)
  {
    return;
  }

  if (type == MDtype::DOUBLE)
  {
    const double v = double_uniform_value;
    clear();
    set_type(MDtype::DOUBLE);
    double_values.resize(length, v);
    is_uniform = false;
  }
  else if (type == MDtype::EXTENDED)
  {
#ifdef DEVSIM_EXTENDED_PRECISION
    const float128 v = float128_uniform_value;
    clear();
    set_type(MDtype::EXTENDED);
    float128_values.resize(length, v);
    is_uniform = false;
#endif
  }
}

void ModelDataHolder::clear() const
{
  type = MDtype::DOUBLE;
  double_uniform_value = 0.0;
  std::vector<double>().swap(double_values);
#ifdef DEVSIM_EXTENDED_PRECISION
  float128_uniform_value = 0.0;
  std::vector<float128>().swap(float128_values);
#endif
  is_uniform = true;
}

bool ModelDataHolder::IsZero() const
{
  bool ret = is_uniform && (double_uniform_value == 0.0);
  return ret;
}

bool ModelDataHolder::IsOne() const
{
  bool ret = is_uniform && (double_uniform_value == 1.0);
  return ret;
}

template <>
const double &ModelDataHolder::GetUniformValue()
{
  return double_uniform_value;
}

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
const float128 &ModelDataHolder::GetUniformValue()
{
  return float128_uniform_value;
}
#endif

template <>
ModelDataHolder::MDtype ModelDataHolder::GetMDtype<double>() const
{
  return ModelDataHolder::MDtype::DOUBLE;
}

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
ModelDataHolder::MDtype ModelDataHolder::GetMDtype<float128>() const
{
  return ModelDataHolder::MDtype::EXTENDED;
}
#endif

template <typename DoubleType>
void ModelDataHolder::SetUniformValue(const DoubleType &v)
{
  clear();
  type = GetMDtype<DoubleType>();
  double_uniform_value   = static_cast<double>(v);
#ifdef DEVSIM_EXTENDED_PRECISION
  float128_uniform_value = static_cast<float128>(v);
#endif
  is_uniform = true;
}

template void ModelDataHolder::SetUniformValue(const double &);
#ifdef DEVSIM_EXTENDED_PRECISION
template void ModelDataHolder::SetUniformValue(const float128 &);
#endif

template <>
const std::vector<double> &ModelDataHolder::GetValues() const
{
  expand_uniform();
#ifdef DEVSIM_EXTENDED_PRECISION
  if (type == MDtype::EXTENDED && double_values.empty())
  {
    double_values.resize(length);
    for (size_t i = 0; i < float128_values.size(); ++i)
    {
      double_values[i] = static_cast<double>(float128_values[i]);
    }
  }
#endif
  return double_values;
}

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
const std::vector<float128> &ModelDataHolder::GetValues() const
{
  expand_uniform();
  if (type == MDtype::DOUBLE && float128_values.empty())
  {
    float128_values.resize(length);
    for (size_t i = 0; i < double_values.size(); ++i)
    {
      float128_values[i] = double_values[i];
    }
  }
  return float128_values;
}
#endif

template <>
void ModelDataHolder::set_indexes(const std::vector<size_t> &indexes, const double &v)
{
  clear();

  double_values.resize(length);

  for (auto i : indexes)
  {
    double_values[i] = v;
  }

  type = MDtype::DOUBLE;
  is_uniform = false;

}

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
void ModelDataHolder::set_indexes(const std::vector<size_t> &indexes, const float128 &v)
{
  clear();

  float128_values.resize(length);

  for (auto i : indexes)
  {
    float128_values[i] = v;
  }

  type = MDtype::EXTENDED;
  is_uniform = false;
}
#endif

template <>
void ModelDataHolder::set_indexes(const std::vector<size_t> &indexes, const std::vector<double> &v)
{
  clear();

  double_values.resize(length);

  for (auto i : indexes)
  {
    double_values[i] = v[i];
  }

  type = MDtype::DOUBLE;
  is_uniform = false;
}

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
void ModelDataHolder::set_indexes(const std::vector<size_t> &indexes, const std::vector<float128> &v)
{
  clear();

  float128_values.resize(length);

  for (auto i : indexes)
  {
    float128_values[i] = v[i];
  }

  type = MDtype::EXTENDED;
  is_uniform = false;
}
#endif

template <>
void ModelDataHolder::set_values(const std::vector<double> &nv)
{
  clear_type(MDtype::EXTENDED);
  type = MDtype::DOUBLE;
  double_values = nv;
  is_uniform = false;
}

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
void ModelDataHolder::set_values(const std::vector<float128> &nv)
{
  clear_type(MDtype::DOUBLE);
  type = MDtype::EXTENDED;
  float128_values = nv;
  is_uniform = false;
}
#endif

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
void ModelDataHolder::SetValue(size_t index, const float128 &nv)
{
  if (index >= length)
  {
    return;
  }

  // promote types if necessary
  set_type(MDtype::EXTENDED);

  expand_uniform();

  float128_values[index] = nv;

}
#endif

template <>
void ModelDataHolder::SetValue(size_t index, const double &nv)
{
  if (index >= length)
  {
    return;
  }

  if (type == MDtype::EXTENDED)
  {
#ifdef DEVSIM_EXTENDED_PRECISION
    SetValue(index, static_cast<float128>(nv));
#endif
  }
  else if (type == MDtype::DOUBLE)
  {
    expand_uniform();

    double_values[index] = nv;
  }
}

