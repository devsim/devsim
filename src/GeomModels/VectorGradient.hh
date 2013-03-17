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

#ifndef VECTOR_GRADIENT_HH
#define VECTOR_GRADIENT_HH
#include "NodeModel.hh"

class Node;
typedef Node *NodePtr;
typedef const Node *ConstNodePtr;

#if 0
class Triangle;
typedef Triangle *TrianglePtr;
typedef const Triangle *ConstTrianglePtr;
#endif

class VectorGradient : public NodeModel
{
    public:
      enum CalcType {DEFAULT=0, AVOIDZERO};
      VectorGradient(RegionPtr, const std::string &, CalcType);

      void Serialize(std::ostream &) const;

    private:
      double calcVectorGradient(ConstNodePtr) const;
      void   calcNodeScalarValues() const;
      void   calc1d() const;
      void   calc2d() const;
      void   calc3d() const;
      void   setInitialValues();

      std::string   parentname_;
      CalcType      calctype_;
      WeakNodeModelPtr yfield_;
      WeakNodeModelPtr zfield_;

      const static char *CalcTypeString[];
};

#endif
