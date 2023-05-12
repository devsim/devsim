/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

/*
@article{doi:10.1063/1.89697,
author = {Joyce,W. B.  and Dixon,R. W. },
title = {Analytic approximations for the Fermi energy of an ideal Fermi gas},
journal = {Applied Physics Letters},
volume = {31},
number = {5},
pages = {354-356},
year = {1977},
doi = {10.1063/1.89697},
URL = { https://doi.org/10.1063/1.89697 },
eprint = { https://doi.org/10.1063/1.89697 }
}

*/

#include "Fermi.hh"
#include "BoostConstants.hh"
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
#endif
#include <cmath>
using std::abs;
#include <cstdlib>
namespace
{
// TODO: "These coefficients only worked out for amd64 in current configuration"
const double breakpoint = 8.46296036621705;
const double offset = -0.0137050034663995;

template <typename T>
T Joyce(T r)
{
    //static const T sqrt1_8 = 1.0/sqrt(8.);
    static const T sqrt1_8 = 0.5 * boost::math::constants::one_div_root_pi<T>();
    static const T c1 = -4.9500897298752622e-3;
    static const T c2 =  1.4838577128872821e-4;
    static const T c3 = -4.4256301190009895e-6;

    T R = r;
    T sum = log(r);
    sum += sqrt1_8 * R;
    R *= r;
    sum += c1 * R;
    R *= r;
    sum += c2 * R;
    R *= r;
    sum += c3 * R;

    return sum;
}

template <typename T>
T dJoycedx(T r)
{
    //static const double sqrt1_8 = 1.0/sqrt(8.);
    static const T sqrt1_8 = 0.5 * boost::math::constants::one_div_root_pi<T>();
    static const T c1 = -2.0*4.9500897298752622e-3;
    static const T c2 =  3.0*1.4838577128872821e-4;
    static const T c3 = -4.0*4.4256301190009895e-6;

    T sum = 1/r;
    T R = r;
    sum += sqrt1_8;
    sum += c1 * R;
    R *= r;
    sum += c2 * R;
    R *= r;
    sum += c3 * R;

    return sum;
}

template <typename T>
T Expansion(T r)
{
    /// M_PI_4 = pi/4
    // static const T d1 = 3./4.* sqrt(M_PI);
    static const T d1 = boost::math::constants::three_quarters_pi<T>();
    //static const T d2 = 4./3.;
    static const T d2 = static_cast<T>(4.)/static_cast<T>(3.);
    //static const T d3 = - M_PI * M_PI/6;
    static const T d3 = - boost::math::constants::pi_sqr_div_six<T>();
    //static const double d1 = 3./4.* sqrt(M_PI);
    //static const double d2 = 4./3.;
    //static const double d3 = - M_PI * M_PI/6;
    T sum = pow(d1*r, d2);
    sum += d3;
    sum = sqrt(sum);

    sum += offset;
    return sum;
}

template <typename T>
T dExpansiondx(T r)
{

        //static const double d0 = sqrt(M_PI)/2.0;
        //static const T d0 = sqrt(M_PI)/2.0;
        static const T d0 = 0.5 * boost::math::constants::root_pi<T>();
        //static const T d1 = 3./4.* sqrt(M_PI);
        static const T d1 = 3./4. * boost::math::constants::root_pi<T>();
        //static const double d2 = 1.0/3.0;
        static const T d2 = boost::math::constants::third<T>();
        T sum = d0 * pow(d1 * r, d2) / Expansion(r);
        return sum;
}

template <typename T>
T good_relerror()
{
}

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
T InvFermi(T r)
{
    if (r < breakpoint)
    {
        return Joyce(r);
    }
    else
    {
        return Expansion(r);
    }
}

template <typename T>
T dInvFermidx(T r)
{
    if (r < breakpoint)
    {
        return dJoycedx(r);
    }
    else
    {
        return dExpansiondx(r);
    }
}

//// Need some way to calculate a function and its derivative simultaneously
//#include <iostream>
template <typename T>
T Fermi(T Eta)
{
    //// initial guess
    T r = exp(Eta);
//    std::cerr << "Init: " << Eta << " " << r << std::endl;
    T rerr = 0.0;
    size_t i = 0;
    do
    {
        const T f = InvFermi(r) - Eta;
        const T fp = dInvFermidx(r);
        T upd = -f/fp;
        while ((upd + r) <= 0.0)
        {
            upd *= 0.5;
        }
        r += upd;
        //// eta would be -infinity for r to go to 0
        rerr = abs(upd)/(abs(r)+good_relerror<T>());
//      std::cerr << "try " << i << " " << r <<  " " << f << " " << fp << " " << upd << " " << rerr << std::endl;
//      TODO: consider second order newton
        ++i;
    } while ((rerr > good_relerror<T>()) && (i < 20) ) ;
    ///// Arbitrary relative error
    return r;

}
///// define dFermidx as g'(x) = 1.0 / (f'(g(x)));
///// This is inefficient so be sure to program special node model depending on ferm result
template <typename T>
T dFermidx(T Eta)
{
    return 1.0/dInvFermidx(Fermi(Eta));
}

template double InvFermi<double>(double);
template double dInvFermidx<double>(double);
template double Fermi<double>(double);
template double dFermidx<double>(double);
#ifdef DEVSIM_EXTENDED_PRECISION
template float128 InvFermi<float128>(float128);
template float128 dInvFermidx<float128>(float128);
template float128 Fermi<float128>(float128);
template float128 dFermidx<float128>(float128);
#endif

#ifdef DEVSIM_UNIT_TEST
#include <iostream>
#include <iomanip>
template <typename T>
T pdiff(T x1, T x2)
{
    return 2.0*(x1-x2)/(x1+x2);
}
template <typename T>
void unit()
{
    std::cout << std::setprecision(std::numeric_limits<T>::max_digits10);
    for (T x = 8.46296036621705; x < 8.46296036621707; x += 0.0000001)
    {
        std::cout
            << x << " "
            << InvFermi(x) << " "
            << Joyce(x) << " "
            << Expansion(x) << " "
            << dJoycedx(x) << " "
            << dExpansiondx(x) <<" "
            << pdiff(dJoycedx(x), dExpansiondx(x)) << " " << Expansion(x)-Joyce(x) <<"\n";
    }
    for (T x = 4.8; x < 4.9; x += 0.001)
    {
        std::cout << x << " " << Fermi(x) << " " << dFermidx(x) << "\n";
    }
}

int main()
{
  unit<double>();
#ifdef DEVSIM_EXTENDED_PRECISION
  unit<float128>();
#endif
}
#endif

