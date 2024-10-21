/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef ATCONTACTNODE_HH
#define ATCONTACTNODE_HH
#include "NodeModel.hh"
template <typename DoubleType>
class AtContactNode : public NodeModel
{
    public:
      void Serialize(std::ostream &) const;

    private:
      friend class dsModelFactory<AtContactNode<DoubleType>>;
      explicit AtContactNode(RegionPtr);
      DoubleType calcNodeVolume(ConstNodePtr) const;
      void calcNodeScalarValues() const;
      void setInitialValues();
};
#endif

