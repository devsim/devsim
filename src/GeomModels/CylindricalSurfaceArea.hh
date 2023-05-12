/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef CYLINDRICAL_SURFACE_AREA_HH
#define CYLINDRICAL_SURFACE_AREA_HH
#include "NodeModel.hh"

class Contact;
class Interface;

NodeModelPtr CreateCylindricalSurfaceArea(RegionPtr /*rp*/);

template <typename DoubleType>
class CylindricalSurfaceArea : public NodeModel {
    public:
        void Serialize(std::ostream &) const;

        CylindricalSurfaceArea(RegionPtr /*rp*/);

    private:
        CylindricalSurfaceArea();
        CylindricalSurfaceArea(const CylindricalSurfaceArea &);
        CylindricalSurfaceArea &operator=(const CylindricalSurfaceArea &);
        void calcCylindricalSurfaceArea2d() const;
        void calcNodeScalarValues() const;
        void setInitialValues();
};
#endif
