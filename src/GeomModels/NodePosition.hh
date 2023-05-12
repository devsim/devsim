/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef NODEPOSITION_HH
#define NODEPOSITION_HH
#include "NodeModel.hh"
template <typename DoubleType>
class NodePosition : public NodeModel
{
    public:
      NodePosition(RegionPtr);
      ~NodePosition();

      void Serialize(std::ostream &) const;

    private:
      DoubleType calcNodePosition(ConstNodePtr) const;
      void calcNodeScalarValues() const;
      void setInitialValues();

      WeakNodeModelPtr yposition;
      WeakNodeModelPtr zposition;
      WeakNodeModelPtr node_index;
      WeakNodeModelPtr coordinate_index;
};

#endif
