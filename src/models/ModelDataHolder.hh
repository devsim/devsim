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

#ifndef MODELDATAHOLDER_HH
#define MODELDATAHOLDER_HH

#include <vector>
#include <cstddef>

class ModelDataHolder
{
  enum MDtype {DOUBLE};
  public:
    explicit ModelDataHolder(size_t l) : double_uniform_value(0.0), length(l), type(DOUBLE), is_uniform(true)
    {
    }

    ModelDataHolder();

    bool IsOne() const;

    bool IsZero() const;

    size_t GetLength() const
    {
      return length;
    }

    bool IsUniform() const;

    template <typename DoubleType>
    const std::vector<DoubleType> &GetValues() const;

    template <typename DoubleType>
    const DoubleType &GetUniformValue();

    template <typename DoubleType>
    void SetUniformValue(const DoubleType &);

    template <typename DoubleType>
    void SetValue(size_t, const DoubleType &);

    template <typename DoubleType>
    std::vector<DoubleType> &Values();

    template <typename DoubleType>
    void set_indexes(const std::vector<size_t> &/*indexes*/, std::vector<DoubleType> &/*values*/);

    template <typename DoubleType>
    void set_indexes(const std::vector<size_t> &/*indexes*/, const DoubleType &/*v*/);

    template <typename DoubleType>
    void set_values(const DoubleType &/*v*/);

    void expand_uniform() const;
    
    void clear();

    void clear_values();

  private:
    mutable double              double_uniform_value;
    mutable std::vector<double> double_values;
    const size_t                length;
    mutable MDtype              type;
    bool                        is_uniform;
};

#endif

