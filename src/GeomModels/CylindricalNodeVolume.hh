/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef CYLINDRICALNODEVOLUME_HH
#define CYLINDRICALNODEVOLUME_HH
#include "NodeModel.hh"
NodeModelPtr CreateCylindricalNodeVolume(RegionPtr);

template <typename DoubleType>
class CylindricalNodeVolume : public NodeModel
{
    public:

      void Serialize(std::ostream &) const;

      CylindricalNodeVolume(RegionPtr);

    private:

      void calcNodeScalarValues() const;
      void setInitialValues();
};
#endif

