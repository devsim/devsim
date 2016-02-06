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

#ifndef VECTOR_HH
#define VECTOR_HH
#include <cstddef>
#include <iostream>
#include <cmath>
/**
    This is your basic vector class providing algebraic operations for data
    1, 2, or 3 dimensions
*/
class Vector {
    public:
//      explicit Vector(double x = 0.0, double y = 0.0, double z = 0.0);
        explicit Vector(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}

        Vector(const Vector &v) : x(v.x), y(v.y), z(v.z)
        {
        }

        Vector & operator=(const Vector &v)
        {
             x  =v.x;
             y  =v.y;
             z  =v.z;
             return *this;
        }


        Vector   operator-(const Vector &v) const
        {
          Vector tmp(*this);
          tmp -= v;
          return tmp;
        }

        Vector   operator-() const
        {
          Vector ret(-x, -y, -z);
          return ret;
        }

        Vector & operator-=(const Vector &v)
        {
          x -= v.x;
          y -= v.y;
          z -= v.z;
          return *this;
        }

        Vector   operator+(const Vector &v) const
        {
          Vector tmp(*this);
          tmp += v;
          return tmp;
        }

        Vector & operator+=(const Vector &v)
        {
          x += v.x;
          y += v.y;
          z += v.z;
          return *this;
        }

        Vector   operator* (double d) const
        {
            Vector ret(*this);
            ret *= d;
            return ret;
        }

        Vector & operator*=(double d)
        {
          x *= d;
          y *= d;
          z *= d;
          return *this;
        }

        Vector   operator/ (double d) const
        {
            Vector ret(*this);
            ret /= d;
            return ret;
        }

        Vector & operator/=(double d)
        {
            x /= d;
            y /= d;
            z /= d;
            return *this;
        }

        double magnitude() const
        {
          return std::sqrt(x*x + y*y + z*z);
        }

        double dot_prod(const Vector &v) const
        {
          return (x * v.x + y*v.y + z*v.z);
        }

        Vector cross_prod(const Vector &v) const
        {
          const double xcomp = (y * v.z) - (z * v.y);
          const double ycomp = (z * v.x) - (x * v.z);
          const double zcomp = (x * v.y) - (y * v.x);
          return Vector(xcomp, ycomp, zcomp);
        }

        const double &Getx() const
        {
            return x;
        }

        const double &Gety() const
        {
            return y;
        }

        const double &Getz() const
        {
            return z;
        }

        void Setx(double xn)
        {
          x = xn;
        }
        void Sety(double yn)
        {
          y = yn;
        }
        void Setz(double zn)
        {
          z = zn;
        }
        
    private:
       double x;
       double y;
       double z;
};

inline Vector operator-(Vector &v1, Vector &v2)
{
   Vector vec(v1);
   vec -= v2;
   return vec;
}

inline double magnitude(const Vector &vec) {
   return vec.magnitude();
}

inline Vector cross_prod(const Vector &v1, const Vector &v2) {
    return v1.cross_prod(v2);
}

inline double dot_prod(const Vector &v1, const Vector &v2) {
    return v1.dot_prod(v2);
}

inline std::ostream & operator<< (std::ostream &os,  const Vector &v)
{
    os << "(" << v.Getx() << ", " << v.Gety() << ", " << v.Getz() << ")";
    return os;
}

inline Vector operator*(double x, const Vector &vec)
{
  return vec * x;
}

#endif
