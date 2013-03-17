/***
DEVSIM
Copyright 2013 Devsim LLC

This file is part of DEVSIM.

DEVSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, version 3.

DEVSIM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with DEVSIM.  If not, see <http://www.gnu.org/licenses/>.
***/

#include "Vector.hh"
#include <cmath>
#include "dsAssert.hh"

/*
Vector::Vector(double x, double y, double z) : x(x), y(y), z(z)
{
}

Vector::Vector(const Vector &v) : x(v.x), y(v.y), z(v.z)
{
}

Vector & Vector::operator=(const Vector &v)
{
     x  =v.x;
     y  =v.y;
     z  =v.z;
     return *this;
}

Vector Vector::operator-() const
{
  Vector ret(-x, -y, -z);
  return ret;
}

Vector Vector::operator-(const Vector &v) const
{
    Vector tmp(*this);
    tmp -= v;
    return tmp;
}

Vector &Vector::operator-=(const Vector &v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

Vector Vector::operator+(const Vector &v) const
{
    Vector tmp(*this);
    tmp += v;
    return tmp;
}

Vector &Vector::operator+=(const Vector &v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

double Vector::magnitude() const
{
    return std::sqrt(x*x + y*y + z*z);
}

Vector Vector::cross_prod(const Vector &v) const {
  // generic formula for 3 dimensions
  const double xcomp = (y * v.z) - (z * v.y);
  const double ycomp = (z * v.x) - (x * v.z);
  const double zcomp = (x * v.y) - (y * v.x);
  return Vector(xcomp, ycomp, zcomp);
}

double Vector::dot_prod(const Vector &v) const {
    // generic formula
    double val = x * v.x + y*v.y + z*v.z;
    return val;
}

Vector Vector::operator*(double d) const
{
    Vector ret(*this);
    ret *= d;
    return ret;
}

Vector &Vector::operator*=(double d) 
{
    x *= d;
    y *= d;
    z *= d;
    return *this;
}

Vector &Vector::operator/=(double d) 
{
    x /= d;
    y /= d;
    z /= d;
    return *this;
}

*/
