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

    private:
      friend class dsModelFactory<CylindricalNodeVolume<DoubleType>>;
      explicit CylindricalNodeVolume(RegionPtr);

      void calcNodeScalarValues() const;
      void setInitialValues();
};
#endif

