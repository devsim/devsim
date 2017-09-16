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

#include "Fermi.hh"
#include <cmath>
using std::abs;
#include <cstdlib>
#ifndef _WIN32
#warning "Extended Precision Version"
#endif
namespace
{
// TODO: "These coefficients only worked out for amd64 in current configuration"
const double breakpoint = 8.46296036621705;
const double offset = -0.0137050034663995;
double Joyce(double r)
{
    static const double sqrt1_8 = 1.0/sqrt(8.);
    static const double c1 = -4.9500897298752622e-3;
    static const double c2 =  1.4838577128872821e-4;
    static const double c3 = -4.4256301190009895e-6; 

    double R = r;
    double sum = log(r);
    sum += sqrt1_8 * R;
    R *= r;
    sum += c1 * R;
    R *= r;
    sum += c2 * R;
    R *= r;
    sum += c3 * R;

    return sum;
}

double dJoycedx(double r)
{
    static const double sqrt1_8 = 1.0/sqrt(8.);
    static const double c1 = -2.0*4.9500897298752622e-3;
    static const double c2 =  3.0*1.4838577128872821e-4;
    static const double c3 = -4.0*4.4256301190009895e-6; 

    double sum = 1/r;
    double R = r;
    sum += sqrt1_8;
    sum += c1 * R;
    R *= r;
    sum += c2 * R;
    R *= r;
    sum += c3 * R;

    return sum;
}

double Expansion(double r)
{
    /// M_PI_4 = pi/4
    static const double d1 = 3./4.* sqrt(M_PI);
    static const double d2 = 4./3.;
    static const double d3 = - M_PI * M_PI/6;
    double sum = pow(d1*r, d2);
    sum += d3;
    sum = sqrt(sum);

    sum += offset;
    return sum;
}

double dExpansiondx(double r)
{

        static const double d0 = sqrt(M_PI)/2.0;
        static const double d1 = 3./4.* sqrt(M_PI);
        static const double d2 = 1.0/3.0;
        double sum = d0 * pow(d1 * r, d2) / Expansion(r);
        return sum;
}
}

double InvFermi(double r)
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

double dInvFermidx(double r)
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
double Fermi(double Eta)
{
    //// initial guess
    double r = exp(Eta);
//    std::cerr << "Init: " << Eta << " " << r << std::endl;
    double rerr = 0.0;
    size_t i = 0;
    do
    {
        const double f = InvFermi(r) - Eta;
        const double fp = dInvFermidx(r);
        double upd = -f/fp;
        while ((upd + r) <= 0.0)
        {
            upd *= 0.5;
        }
        r += upd;
        //// eta would be -infinity for r to go to 0
        rerr = abs(upd)/(abs(r)+1.0e-10);
//      std::cerr << "try " << i << " " << r <<  " " << f << " " << fp << " " << upd << " " << rerr << std::endl;
//      TODO: consider second order newton
        ++i;
    } while ((rerr > 1.0e-13) && (i < 20) ) ;
    ///// Arbitrary relative error
    return r;

}
///// define dFermidx as g'(x) = 1.0 / (f'(g(x)));
///// This is inefficient so be sure to program special node model depending on ferm result
double dFermidx(double Eta)
{
    return 1.0/dInvFermidx(Fermi(Eta));
}

#ifdef MYUNIT
#include <iostream>
#include <iomanip>
double pdiff(double x1, double x2)
{
    return 2.0*(x1-x2)/(x1+x2);
}
int main()
{
    std::cout << std::setprecision(15);
    for (double x = 8.46296036621705; x < 8.46296036621707; x += 0.0000001)
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
    for (double x = 4.8; x < 4.9; x += 0.001)
    {
        std::cout << x << " " << Fermi(x) << " " << dFermidx(x) << "\n";
    }
}
#endif

