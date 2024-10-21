/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef CYLINDRICALEDGENODEVOLUME_HH
#define CYLINDRICALEDGENODEVOLUME_HH
#include "EdgeModel.hh"

EdgeModelPtr CreateCylindricalEdgeNodeVolume(RegionPtr);

template <typename DoubleType>
class CylindricalEdgeNodeVolume : public EdgeModel
{
    public:

      void Serialize(std::ostream &) const;

    private:

      friend class dsModelFactory<CylindricalEdgeNodeVolume>;
      explicit CylindricalEdgeNodeVolume(RegionPtr);

      void derived_init();

      void calcEdgeScalarValues() const;

      WeakEdgeModelPtr node1Volume_;
};
#endif

