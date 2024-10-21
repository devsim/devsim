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
      void Serialize(std::ostream &) const;

    private:
      friend class dsModelFactory<NodeVolume<DoubleType>>;
      explicit NodeVolume(RegionPtr);

      DoubleType calcNodeVolume(ConstNodePtr) const;
      void calcNodeScalarValues() const;
      void setInitialValues();
};
#endif

