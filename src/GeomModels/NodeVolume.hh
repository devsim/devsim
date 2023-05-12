/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef NODEVOLUME_HH
#define NODEVOLUME_HH
#include "NodeModel.hh"

template <typename DoubleType>
class NodeVolume : public NodeModel
{
    public:
      NodeVolume(RegionPtr);

      void Serialize(std::ostream &) const;

    private:
      DoubleType calcNodeVolume(ConstNodePtr) const;
      void calcNodeScalarValues() const;
      void setInitialValues();
};
#endif

