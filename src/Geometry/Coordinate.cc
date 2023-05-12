/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "Coordinate.hh"
#include "Vector.hh"

#include <cstddef>

using namespace std;

Coordinate::Coordinate(double x, double y, double z) : position(x, y, z), index(0)
{
}

const Vector<double> &Coordinate::Position() const
{
   return position;
}


