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

#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
class kahan {
    public:
        explicit kahan(double v) : value_(v), correction_(0.0) {}
        kahan(const kahan & v) : value_(v.value_), correction_(v.correction_) {}


        kahan &operator+=(double v)
        {
            if (fabs(value_) < fabs(correction_))
            {
              const double t = value_;
              value_ = correction_;
              correction_ = t;
            }

            const double x = value_ + v;
            double c = x;
            if (fabs(value_) < fabs(v))
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
        kahan &operator-=(double v)
        {
            (*this) += (-v);
            return *this;
        }

        ~kahan() {};

        operator double()
        {
            return value_ + correction_;
        }

        double value_;
        double correction_;

    private:
        kahan();
        kahan &operator=(const kahan &);
};

std::ostream &operator<<(std::ostream &obj, const kahan &k)
{
    obj <<  "(" << k.value_ << ", " << k.correction_ << ")";
    return obj;
}

double kahan3(double a, double b, double c)
{
  kahan k(a);
  k += b;
  k += c;
  return static_cast<double>(k);
}

double kahan4(double a, double b, double c, double d)
{
  kahan k(a);
  k += b;
  k += c;
  k += d;
  return static_cast<double>(k);
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
        std::cout << v << " " << a << " " << static_cast<double>(a) << " " << b << std::endl;
    }


//    std::cout << 
}
#endif
