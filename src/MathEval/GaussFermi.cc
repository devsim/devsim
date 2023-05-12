/***
DEVSIM
Copyright 2021 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/
#include "GaussFermi.hh"
#include "BoostConstants.hh"
#include "MiscMathFunc.hh"
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
#endif
#include <utility>
#include <cmath>
using std::abs;

#ifdef DEVSIM_UNIT_TEST
#include <iostream>
#include <iomanip>
#endif

/*
Implementation of:
https://doi.org/10.1063/1.3374475
@article{doi:10.1063/1.3374475,
author = {Paasch,Gernot  and Scheinert,Susanne },
title = {Charge carrier density of organics with Gaussian density of states: Analytical approximation for the Gauss–Fermi integral},
journal = {Journal of Applied Physics},
volume = {107},
number = {10},
pages = {104501},
year = {2010},
doi = {10.1063/1.3374475},
URL = { https://doi.org/10.1063/1.3374475 },
eprint = { https://doi.org/10.1063/1.3374475 }
}
*/

namespace {

//// Fix multiprecision constexpr issue
//#if BOOST_VERSION / 100 >= 1072
//recurrent issue with constexpr
#if 0
template <typename T>
struct MC {
    static constexpr T sqrt2 = boost::math::constants::root_two<T>();
    static constexpr T sqrt2_pi = boost::math::constants::root_two<T>() * boost::math::constants::one_div_root_pi<T>();
    static constexpr T one_div_root_two_pi = boost::math::constants::one_div_root_two_pi<T>();
};
#else
template <typename T>
struct MC {
    const static inline T sqrt2 = boost::math::constants::root_two<T>();
    const static inline T sqrt2_pi = boost::math::constants::root_two<T>() * boost::math::constants::one_div_root_pi<T>();
    const static inline T one_div_root_two_pi = boost::math::constants::one_div_root_two_pi<T>();
};
#endif

template <typename T>
inline T calcH_Impl(const T &s, const T &S)
{
    const T &sqrt2 = MC<T>::sqrt2;

    T H = sqrt2 / s * erfc_inv(exp(-0.5 * S));
#ifdef DEVSIM_UNIT_TEST
    std::cout << std::setprecision(std::numeric_limits<T>::max_digits10);
    std::cout << "DEBUG NEW H " << s << " " << H << "\n";
#endif
    return H;
}

template <typename T>
inline T calcH(const T &s, const T &S)
{
    thread_local auto p = std::make_pair(s, calcH_Impl(s, S));

    // Assume that s is constant across the same call in the same region
    if (p.first != s)
    {
      p = std::make_pair(s, calcH_Impl(s, S));
    }
#if 0
    else
    {
      std::cout << "DEBUG REUSE H\n";
    }
#endif

    return p.second;
}

template <typename T>
inline T calcK(const T &s, const T &S, const T &H)
{
  const T &sqrt2_pi = MC<T>::sqrt2_pi;

  T K = 2 * (1. - H / s * sqrt2_pi * exp(0.5 * S * (1. - H * H)));
  return K;
}

}

template <typename T>
T gfi(T zeta, T s)
{
    const T &sqrt2 = MC<T>::sqrt2;
    const T S = s * s;

    T H = calcH(s, S);

    T value;
    if (zeta < -S)
    {
        const T K = calcK(s, S, H);
        value = exp(0.5 * S + zeta) / (exp(K*(zeta+S)) + 1);
    }
    else
    {
        value = 0.5 * erfc(-zeta / (s*sqrt2) * H);
    }

    return value;
}

template <typename T>
T dgfidx(T zeta, T s)
{
    const T S = s * s;

    T H = calcH(s, S);

    T dvalue;
    if (zeta < -S)
    {
        const T K = calcK(s, S, H);
        const T den_inv = 1. / (exp(K * (S + zeta)) + 1.);
        dvalue = exp(0.5 * S + zeta) * den_inv * (1. - K*exp(K * (S+zeta)) * den_inv);
    }
    else
    {
        const T &one_div_root_two_pi = MC<T>::one_div_root_two_pi;
        dvalue = one_div_root_two_pi * H / s * exp(-0.5 * pow(H*zeta,2)/S);
    }

    return dvalue;
}

//### inverse function for Gaussian Fermi Integral

namespace {
template <typename T>
T good_relerror();

template <>
double good_relerror()
{
  return 1e-12;
}

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
float128 good_relerror()
{
  return 1e-31;
}
#endif
}

template <typename T>
T igfi(T g, T s)
{
    // using the Newton method
    // The initial guess
    // perhaps the degenerate approximation
    // or provided from the last call
    // improves speed
    T x = 0.0;
    //printf("%d %1.15e %1.15e\n", -1, x, rerr);
#ifdef DEVSIM_UNIT_TEST
    std::cout << std::setprecision(std::numeric_limits<T>::max_digits10);
#endif
    T arg = 1. - 2. * g;

    static const T bound = 1.0 - std::numeric_limits<T>::epsilon();

    // prevent infinite results
    if (arg <= -1.0)
    {
      arg = -bound;
    }
    else if (arg >= 1.0)
    {
      arg = bound;
    }

    const T &sqrt2 = MC<T>::sqrt2;
    // using the degenerate approximation
    const T S = s*s;
    const T H = calcH(s, S);
    x = -s * sqrt2 * erf_inv(arg) / H;

#if 0
    // slightly less accuracy, best to use iteration
    if ((x >= -S) && (abs(arg) != bound))
    {
        return x;
    }
#endif

    T rerr = 0.0;
    size_t i = 0;
    T f;
    T fp;
    T upd;

    do
    {
        f = gfi(x, s) - g;
        fp = dgfidx(x, s);
        upd = -f / fp;
        x += upd;
        rerr = abs(upd)/(abs(x) + good_relerror<T>());
#ifdef DEVSIM_UNIT_TEST
    std::cout << i << " " << x << " " << rerr << "\n";
#endif
        ++i;
    } while ((rerr > good_relerror<T>()) && (i < 200));
    return x;
}


template <typename T>
T digfidx(T g, T s)
{
    return 1.0 / dgfidx(igfi(g,s), s);
}

template double gfi<double>(double, double);
template double dgfidx<double>(double, double);
template double igfi<double>(double, double);
template double digfidx<double>(double, double);

#ifdef DEVSIM_EXTENDED_PRECISION
template float128 gfi<float128>(float128, float128);
template float128 dgfidx<float128>(float128, float128);
template float128 igfi<float128>(float128, float128);
template float128 digfidx<float128>(float128, float128);
#endif

#ifdef DEVSIM_UNIT_TEST
template <typename DoubleType>
void unit()
{
  for (size_t j = 2; j <= 8; j += 2)
  {
    DoubleType s = static_cast<DoubleType>(j);
    for (size_t i = 0; i <= 10; ++i)
    {
      DoubleType zeta = 7 * i - 70.;
      DoubleType g= gfi(zeta,s);
      DoubleType dg= dgfidx(zeta,s);
      DoubleType ginv= igfi(g,s);
      DoubleType dginv= digfidx(g,s);
      std::cout
                << zeta << " "
                << g << " "
                << dg << " "
                << ginv << " "
                << dginv << " " << 1.0/dginv << "\n";
    }
  }
#if 0
  DoubleType k_B = 1.380649e-23;
  DoubleType e = 1.602176634e-19;
  DoubleType T = 300;
  DoubleType sigma = 0.10;
  DoubleType s = (sigma*e)/(k_B*T);
  DoubleType S = s*s;
  DoubleType V_0 = 0;
  DoubleType V_f = V_0-S*k_B*T/e;// # (-0.65372119-1.2) ;
  DoubleType zeta = (V_f*e-V_0*e)/(k_B*T);
  // zeta =-0.5
  DoubleType zeta_1 = (V_f*e-V_0*e-pow(sigma*e, 2)/k_B*T)/(k_B*T);

  DoubleType g= gfi(zeta,s);
  DoubleType dg= dgfidx(zeta,s);
  DoubleType ginv= igfi(g,s);
  DoubleType dginv= digfidx(g,s);
// print(g,dg,ginv)
  std::cout << std::setprecision(std::numeric_limits<DoubleType>::max_digits10);
  std::cout
            << " zeta\t" << zeta << "\n"
            << " gfi\t" << g << "\n"
            << " dgfidx\t" << dg << "\n"
            << " igfi\t" << ginv << "\n"
            << " digfidx\t" << dginv << " " << 1.0/dginv << "\n";
#endif
}

int main()
{
  unit<double>();
#ifdef DEVSIM_EXTENDED_PRECISION
  unit<float128>();
#endif
}
#endif
