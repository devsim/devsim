/***
DEVSIM
Copyright 2017 Devsim LLC

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

#include "ModelDataHolder.hh"

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

  if (type == DOUBLE)
  {
    double_values.clear();
    double_values.resize(length, double_uniform_value);
  }
}

void ModelDataHolder::clear()
{
  switch (type)
  {
    case DOUBLE:
      double_uniform_value = 0.0;
      double_values.clear();
      break;
  }
  is_uniform = true;
}

bool ModelDataHolder::IsZero() const
{
  bool ret = false;
  switch (type)
  {
    case DOUBLE:
      ret = is_uniform && (double_uniform_value == 0.0);
      break;
  }

  return ret;
}

bool ModelDataHolder::IsOne() const
{
  bool ret = false;
  switch (type)
  {
    case DOUBLE:
      ret = is_uniform && (double_uniform_value == 1.0);
      break;
  }

  return ret;
}

template <>
const double &ModelDataHolder::GetUniformValue()
{
  if (type == DOUBLE)
  {
    return double_uniform_value;
  }
  else
  {
// error
  }
  return double_uniform_value;
}

template <>
void ModelDataHolder::SetUniformValue(const double &v)
{
  is_uniform = true;
  double_uniform_value = v;
  double_values.clear();
}

template <>
const std::vector<double> &ModelDataHolder::GetValues() const
{
  expand_uniform();
  type = DOUBLE;
  return double_values;
}

template <>
void ModelDataHolder::set_indexes(const std::vector<size_t> &indexes, const double &v)
{
  double_values.clear();
  double_values.resize(length);

  for (auto i : indexes)
  {
    double_values[i] = v;
  }

  is_uniform = false;
}

template <>
void ModelDataHolder::set_indexes(const std::vector<size_t> &indexes, const std::vector<double> &v)
{
  double_values.clear();
  double_values.resize(length);

  for (auto i : indexes)
  {
    double_values[i] = v[i];
  }

  is_uniform = false;
}

template <>
void ModelDataHolder::set_values(const std::vector<double> &nv)
{
  type = DOUBLE;
  double_values = nv;
  is_uniform = false;
}

template <>
void ModelDataHolder::SetValue(size_t index, const double &nv)
{
  type = DOUBLE;
  expand_uniform();
  if (index < length)
  {
    double_values[index] = nv;
  }
  is_uniform = false;
}

