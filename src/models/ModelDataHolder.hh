/***
DEVSIM
Copyright 2017 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef MODELDATAHOLDER_HH
#define MODELDATAHOLDER_HH

#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
#endif

#include <vector>
#include <cstddef>


class ModelDataHolder
{
  enum class MDtype {DOUBLE, EXTENDED};

  public:
    explicit ModelDataHolder(size_t l) : double_uniform_value(0.0), length(l), type(MDtype::DOUBLE), is_uniform(true)
    {
      // default float128 are 0.0
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
    MDtype GetMDtype() const;

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

    void clear() const;

    void expand_uniform() const;

  private:

    void clear_type(MDtype t) const;
    void set_type(MDtype t) const;

    mutable std::vector<double> double_values;
    mutable double              double_uniform_value;
#ifdef DEVSIM_EXTENDED_PRECISION
    mutable float128              float128_uniform_value;
    mutable std::vector<float128> float128_values;
#endif
    const size_t                length;
    mutable MDtype              type;
    mutable bool                is_uniform;
};

#endif

