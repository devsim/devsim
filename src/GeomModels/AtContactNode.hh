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
      AtContactNode(RegionPtr);
      void Serialize(std::ostream &) const;

    private:
      DoubleType calcNodeVolume(ConstNodePtr) const;
      void calcNodeScalarValues() const;
      void setInitialValues();
};
#endif

