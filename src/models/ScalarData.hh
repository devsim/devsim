/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef SCALAR_DATA_HH
#define SCALAR_DATA_HH
#include <vector>
#include <cstddef>

namespace ScalarDataHelper {

//// Someday /= as well for pow(x,-1)
template <typename DoubleType>
struct times_equal {
  void operator()(DoubleType &x, const DoubleType &y) const
  {
    x *= y;
  }
};

template <typename DoubleType>
struct plus_equal {
  void operator()(DoubleType &x, const DoubleType &y) const
  {
    x += y;
  }
};

template <typename DoubleType>
struct minus_equal {
  void operator()(DoubleType &x, const DoubleType &y) const
  {
    x -= y;
  }
};

#if 0
template <typename DoubleType>
struct times_equal {
  DoubleType &operator()(DoubleType &x, const DoubleType &y) const
  {
    x *= y;
    return x;
  }
};

template <typename DoubleType>
struct plus_equal {
  DoubleType &operator()(DoubleType &x, const DoubleType &y) const
  {
    x += y;
    return x;
  }
};

template <typename DoubleType>
struct minus_equal {
  DoubleType &operator()(DoubleType &x, const DoubleType &y) const
  {
    x -= y;
    return x;
  }
};
#endif
}

template <typename T, typename DoubleType>
class ScalarData {
    public:
        typedef T reftype;

        // implicit is expensive when data is constant
        ScalarData(DoubleType, size_t);
        explicit ScalarData(const T &);
        ScalarData(const ScalarData &);
        explicit ScalarData(const std::vector<DoubleType> &);

        ScalarData &operator=(const ScalarData &);

        ScalarData &times_equal_data (const ScalarData &);
        ScalarData &times_equal_model(const T &);
        ScalarData &times_equal_scalar(DoubleType);

/*
        ScalarData &operator-=(const ScalarData &);
        ScalarData &operator-=(const T &);
        ScalarData &operator-=(DoubleType);
*/

        ScalarData &plus_equal_data(const ScalarData &);
        ScalarData &plus_equal_model(const T &);
        ScalarData &plus_equal_scalar(DoubleType);

        DoubleType operator[](size_t) const;
        DoubleType &operator[](size_t);
        const std::vector<DoubleType> &GetScalarList() const;

        bool   IsUniform() const {
          return isuniform;
        }

        DoubleType GetUniformValue() const {
          return uniform_value;
        }

        size_t GetLength() const {
          return length;
        }

        //// Use the functors above as helpes (times_equal, etc)
        template <typename V> ScalarData &op_equal_data(const ScalarData &, const V &);
        template <typename V> ScalarData &op_equal_model(const T &, const V &);
        template <typename V> ScalarData &op_equal_scalar(const DoubleType &, const V &);

        bool IsZero() const
        {
          return (isuniform && (uniform_value == 0.0));
        }

        bool IsOne() const
        {
          return (isuniform && (uniform_value == 1.0));
        }

    private:

        void MakeAssignable() const;

        ScalarData() = delete;
        mutable const reftype *refdata;
        mutable std::vector<DoubleType> values;
        mutable bool           isuniform;
        mutable DoubleType     uniform_value;
        size_t                 length;
};
#endif
