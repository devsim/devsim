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
template <typename DoubleType>
class kahan {
    public:
        explicit kahan(DoubleType v) : value_(v), correction_(0.0) {}
        kahan(const kahan & v) : value_(v.value_), correction_(v.correction_) {}


        kahan &operator+=(DoubleType v)
        {
            if (fabs(value_) < fabs(correction_))
            {
              const DoubleType t = value_;
              value_ = correction_;
              correction_ = t;
            }

            const DoubleType x = value_ + v;
            DoubleType c = x;
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
        kahan &operator-=(DoubleType v)
        {
            (*this) += (-v);
            return *this;
        }

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

template <typename DoubleType>
std::ostream &operator<<(std::ostream &obj, const kahan<DoubleType> &k)
{
    obj <<  "(" << k.value_ << ", " << k.correction_ << ")";
    return obj;
}

template <typename DoubleType>
DoubleType kahan3(DoubleType a, DoubleType b, DoubleType c)
{
  kahan<DoubleType> k(a);
  k += b;
  k += c;
  return static_cast<DoubleType>(k);
}

template <typename DoubleType>
DoubleType kahan4(DoubleType a, DoubleType b, DoubleType c, DoubleType d)
{
  kahan<DoubleType> k(a);
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
