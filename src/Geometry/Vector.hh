/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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
template <typename DoubleType>
class Vector {
    public:
//      explicit Vector(DoubleType x = 0.0, DoubleType y = 0.0, DoubleType z = 0.0);
        explicit Vector(DoubleType x = 0, DoubleType y = 0, DoubleType z = 0) : x(x), y(y), z(z) {}

        Vector(const Vector<DoubleType> &v) : x(v.x), y(v.y), z(v.z)
        {
        }

        Vector & operator=(const Vector<DoubleType> &v)
        {
             x  =v.x;
             y  =v.y;
             z  =v.z;
             return *this;
        }


        Vector   operator-(const Vector<DoubleType> &v) const
        {
          Vector<DoubleType> tmp(*this);
          tmp -= v;
          return tmp;
        }

        Vector   operator-() const
        {
          Vector<DoubleType> ret(-x, -y, -z);
          return ret;
        }

        Vector & operator-=(const Vector<DoubleType> &v)
        {
          x -= v.x;
          y -= v.y;
          z -= v.z;
          return *this;
        }

        Vector   operator+(const Vector<DoubleType> &v) const
        {
          Vector<DoubleType> tmp(*this);
          tmp += v;
          return tmp;
        }

        Vector & operator+=(const Vector<DoubleType> &v)
        {
          x += v.x;
          y += v.y;
          z += v.z;
          return *this;
        }

        Vector   operator* (DoubleType d) const
        {
            Vector<DoubleType> ret(*this);
            ret *= d;
            return ret;
        }

        Vector & operator*=(DoubleType d)
        {
          x *= d;
          y *= d;
          z *= d;
          return *this;
        }

        Vector   operator/ (DoubleType d) const
        {
            Vector<DoubleType> ret(*this);
            ret /= d;
            return ret;
        }

        Vector & operator/=(DoubleType d)
        {
            x /= d;
            y /= d;
            z /= d;
            return *this;
        }

        DoubleType magnitude() const
        {
          return sqrt(x*x + y*y + z*z);
        }

        DoubleType dot_prod(const Vector<DoubleType> &v) const
        {
          return (x * v.x + y*v.y + z*v.z);
        }

        Vector cross_prod(const Vector<DoubleType> &v) const
        {
          const DoubleType xcomp = (y * v.z) - (z * v.y);
          const DoubleType ycomp = (z * v.x) - (x * v.z);
          const DoubleType zcomp = (x * v.y) - (y * v.x);
          return Vector<DoubleType>(xcomp, ycomp, zcomp);
        }

        const DoubleType &Getx() const
        {
            return x;
        }

        const DoubleType &Gety() const
        {
            return y;
        }

        const DoubleType &Getz() const
        {
            return z;
        }

        void Setx(DoubleType xn)
        {
          x = xn;
        }
        void Sety(DoubleType yn)
        {
          y = yn;
        }
        void Setz(DoubleType zn)
        {
          z = zn;
        }

    private:
       DoubleType x;
       DoubleType y;
       DoubleType z;
};

template <typename DoubleType>
inline Vector<DoubleType> ConvertVector(const Vector<double> &v)
{
  return Vector<DoubleType>(v.Getx(), v.Gety(), v.Getz());
}

template <typename DoubleType>
inline Vector<DoubleType> operator-(Vector<DoubleType> &v1, Vector<DoubleType> &v2)
{
   Vector<DoubleType> vec(v1);
   vec -= v2;
   return vec;
}

template <typename DoubleType>
inline DoubleType magnitude(const Vector<DoubleType> &vec) {
   return vec.magnitude();
}

template <typename DoubleType>
inline Vector<DoubleType> cross_prod(const Vector<DoubleType> &v1, const Vector<DoubleType> &v2) {
    return v1.cross_prod(v2);
}

template <typename DoubleType>
inline DoubleType dot_prod(const Vector<DoubleType> &v1, const Vector<DoubleType> &v2) {
    return v1.dot_prod(v2);
}

template <typename DoubleType>
inline std::ostream & operator<< (std::ostream &os,  const Vector<DoubleType> &v)
{
    os << "(" << v.Getx() << ", " << v.Gety() << ", " << v.Getz() << ")";
    return os;
}

template <typename DoubleType>
inline Vector<DoubleType> operator*(DoubleType x, const Vector<DoubleType> &vec)
{
  return vec * x;
}

#endif

