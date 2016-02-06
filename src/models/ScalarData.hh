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

#ifndef SCALAR_DATA_HH
#define SCALAR_DATA_HH
#include <vector>
typedef std::vector<double> ScalarList;
#include <cstddef>

namespace ScalarDataHelper {

//// Someday /= as well for pow(x,-1)
struct times_equal {
  void operator()(double &x, const double &y) const
  {
    x *= y;
  }
};

struct plus_equal {
  void operator()(double &x, const double &y) const
  {
    x += y;
  }
};

struct minus_equal {
  void operator()(double &x, const double &y) const
  {
    x -= y;
  }
};
#if 0

struct times_equal {
  double &operator()(double &x, const double &y) const
  {
    x *= y;
    return x;
  }
};

struct plus_equal {
  double &operator()(double &x, const double &y) const
  {
    x += y;
    return x;
  }
};

struct minus_equal {
  double &operator()(double &x, const double &y) const
  {
    x -= y;
    return x;
  }
};
#endif
}

template <typename T>
class ScalarData {
    public:
        typedef T reftype;

        // implicit is expensive when data is constant
        ScalarData(double, size_t);
        explicit ScalarData(const T &);
        ScalarData(const ScalarData &);
        explicit ScalarData(const std::vector<double> &);

        ScalarData &operator=(const ScalarData &);

        ScalarData &operator*=(const ScalarData &);
        ScalarData &operator*=(const T &);
        ScalarData &operator*=(double);

/*
        ScalarData &operator-=(const ScalarData &);
        ScalarData &operator-=(const T &);
        ScalarData &operator-=(double);
*/

        ScalarData &operator+=(const ScalarData &);
        ScalarData &operator+=(const T &);
        ScalarData &operator+=(double);

        double operator[](size_t) const;
        double &operator[](size_t);
        const ScalarList &GetScalarList() const;

        bool   IsUniform() const {
          return isuniform;
        }

        double GetUniformValue() const {
          return uniform_value;
        }

        size_t GetLength() const {
          return length;
        }

        //// Use the functors above as helpes (times_equal, etc)
        template <typename U> ScalarData &op_equal(const ScalarData &, const U &);
        template <typename U> ScalarData &op_equal(const T &, const U &);
        template <typename U> ScalarData &op_equal(const double &, const U &);

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

        ScalarData();
        mutable const reftype *refdata;
        mutable ScalarList     values;
        mutable bool           isuniform;
        mutable double         uniform_value;
        size_t             length;
};
#endif
