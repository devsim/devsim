/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef VECTOR_GRADIENT_HH
#define VECTOR_GRADIENT_HH
#include "NodeModel.hh"

class Node;
typedef Node *NodePtr;
typedef const Node *ConstNodePtr;

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
      friend class dsModelFactory<VectorGradient<DoubleType>>;
      void Serialize(std::ostream &) const;


    private:
      VectorGradient(RegionPtr, const std::string &, VectorGradientEnum::CalcType);

      void derived_init();

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

