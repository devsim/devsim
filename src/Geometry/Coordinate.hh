/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef COORDINATE_HH
#define COORDINATE_HH

#include "Vector.hh"  // Requires definition
#include <cstddef>

class Coordinate;
typedef Coordinate *CoordinatePtr;

class Coordinate {
   public:
      Coordinate(double, double, double);

      const Vector<double> &Position() const;

      size_t GetIndex() const
      {
         return index;
      }

      void setIndex(size_t i)
      {
         index = i;
      }

   private:
      Coordinate();
      Coordinate (const Coordinate &);
      Coordinate &operator=(const Coordinate &);

      Vector<double> position;
      size_t index;
};
#endif
