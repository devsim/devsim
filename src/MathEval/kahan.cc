/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "kahan.hh"
#include <cmath>
using std::abs;


#include <vector>

template <typename DoubleType>
class kahan {
    public:
        explicit kahan(DoubleType v) : value_(v), correction_(0.0) {}
        kahan(const kahan & v) : value_(v.value_), correction_(v.correction_) {}


        kahan &operator+=(DoubleType v);
        kahan &operator-=(DoubleType v);

        ~kahan() {};

        operator DoubleType()
        {
            return value_ + correction_;
        }

        DoubleType value_;
        DoubleType correction_;

    private:
        kahan();
        kahan &operator=(const kahan &);
};

#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
#endif


#if defined(DEVSIM_EXTENDED_PRECISION)
template <typename DoubleType>
struct KahanType;
template <> struct KahanType<double>
{
// better numerical consistency for builds adopting this option
#if defined(USE_CPP_BIN_FLOAT)
  using type = kahan<double>;
#else
  using type = float128;
#endif
};

template <> struct KahanType<float128>
{
  using type = kahan<float128>;
};
#else
template <typename DoubleType>
struct KahanType
{
  using type = kahan<DoubleType>;
};
#endif



template <typename DoubleType>
DoubleType kahan3(DoubleType a, DoubleType b, DoubleType c)
{
  typename KahanType<DoubleType>::type k(a);
  k += b;
  k += c;
  return static_cast<DoubleType>(k);
}

template <typename DoubleType>
DoubleType kahan4(DoubleType a, DoubleType b, DoubleType c, DoubleType d)
{
  typename KahanType<DoubleType>::type k(a);
  k += b;
  k += c;
  k += d;
  return static_cast<DoubleType>(k);
}

#if 0
int main()
{
    std::vector<double> x;
    x.push_back(1e20);
    x.push_back(297.);
    x.push_back(-1e20);
    x.push_back(-1e3);

    std::cout << std::scientific << std::setprecision(15);

    kahan a(0.0);
    double b(0.0);
    for (size_t i = 0; i < x.size(); ++i)
    {
        double v = x[i];
        a += v;
        b += v;
        std::cout << v << " " << a << " " << static_cast<DoubleType>(a) << " " << b << std::endl;
    }


//    std::cout <<
}
#endif

template <typename DoubleType>
kahan<DoubleType> &kahan<DoubleType>::operator+=(DoubleType v)
{
    if (abs(value_) < abs(correction_))
    {
      const DoubleType t = value_;
      value_ = correction_;
      correction_ = t;
    }

    const DoubleType x = value_ + v;
    DoubleType c = x;
    if (abs(value_) < abs(v))
    {
      c -= v;
      c -= value_;
    }
    else
    {
      c -= value_;
      c -= v;
    }
    value_      = x;
    correction_ -= c;
    return *this;
}

template <typename DoubleType>
kahan<DoubleType> &kahan<DoubleType>::operator-=(DoubleType v)
{
    (*this) += (-v);
    return *this;
}

template class kahan<double>;
template double kahan3(double, double, double);
template double kahan4(double, double, double, double);
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class kahan<float128>;
template float128 kahan3(float128, float128, float128);
template float128 kahan4(float128, float128, float128, float128);
#endif


