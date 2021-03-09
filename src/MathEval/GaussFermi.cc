/***
DEVSIM
Copyright 2021 Devsim LLC

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
#include "GaussFermi.hh"
#include "BoostConstants.hh"
#include "MiscMathFunc.hh"
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
#endif
#include <cmath>
using std::abs;

#ifdef DEVSIM_UNIT_TEST
#include <iostream>
#include <iomanip>
#endif

template <typename T>
T gfi(T zeta, T s)
{
    const T S = s * s;
    // S is 
    static const T sqrt2 = boost::math::constants::root_two<T>();
    static const T sqrt2_pi = sqrt2 * boost::math::constants::one_div_root_pi<T>();

    T H = sqrt2 / s * erfc_inv(exp(-0.5 * S));

    T value;
    if (zeta < -S)
    {
        const T K = 2 * (1. - H / s * sqrt2_pi * exp(0.5 * S * (1. - H * H)));
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
    // S is 
    static const T sqrt2 = boost::math::constants::root_two<T>();
    static const T sqrt2_pi = sqrt2 * boost::math::constants::one_div_root_pi<T>();

    T H = sqrt2 / s * erfc_inv(exp(-0.5 * S));

    T dvalue;
    if (zeta < -S)
    {
        const T K = 2 * (1. - H / s * sqrt2_pi * exp(0.5 * S * (1. - H * H)));
        const T den_inv = 1. / (exp(K * (S + zeta)) + 1.);
        dvalue = exp(0.5 * S + zeta) * den_inv * (1. - K*exp(K * (S+zeta)) * den_inv);
    }
    else
    {
        static const T one_div_root_two_pi = boost::math::constants::one_div_root_two_pi<T>();
        dvalue = one_div_root_two_pi * H / s * exp(-0.5 * pow(H*zeta,2)/S);
    }

    return dvalue;
}

//### inverse function for Gaussian Fermi Integral

namespace {
template <typename T>
T good_relerror()
{
}

template <>
double good_relerror()
{
  return 1e-12;
}

template <>
float128 good_relerror()
{
  return 1e-31;
}
}

template <typename T>
T igfi(T g, T s)
{
    // using the Newton method
    // The initial guess
    // perhaps the degenerate approximation
    // or provided from the last call
    // improves speed
    static const T sqrt2 = boost::math::constants::root_two<T>();
    T x = 0.0;
    // This is a good initial guess, but can blow up  
    // x = s * sqrt2 * erf_inv(g*2-1);
    T rerr = 0.0;
    size_t i = 0;
    
    T f;
    T fp;
    T upd;
    //printf("%d %1.15e %1.15e\n", -1, x, rerr);
#ifdef DEVSIM_UNIT_TEST
    std::cout << std::setprecision(std::numeric_limits<T>::max_digits10);
#endif
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

}

int main()
{
  unit<double>();
#ifdef DEVSIM_EXTENDED_PRECISION
  unit<float128>();
#endif
}
#endif
