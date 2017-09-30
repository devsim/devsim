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

namespace VectorGradientEnum
{
  enum CalcType {DEFAULT=0, AVOIDZERO};
  extern const char *CalcTypeString[];
}

NodeModelPtr CreateVectorGradient(RegionPtr, const std::string &, VectorGradientEnum::CalcType);

template <typename DoubleType>
class VectorGradient : public NodeModel
{
    public:

      void Serialize(std::ostream &) const;

      VectorGradient(RegionPtr, const std::string &, VectorGradientEnum::CalcType);

    private:


      DoubleType calcVectorGradient(ConstNodePtr) const;
      void   calcNodeScalarValues() const;
      void   calc1d() const;
      void   calc2d() const;
      void   calc3d() const;
      void   setInitialValues();

      std::string   parentname_;
      VectorGradientEnum::CalcType      calctype_;
      WeakNodeModelPtr yfield_;
      WeakNodeModelPtr zfield_;

};

#endif

