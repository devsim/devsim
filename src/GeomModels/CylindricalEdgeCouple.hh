/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef CYLINDRICALEDGECOUPLE_HH
#define CYLINDRICALEDGECOUPLE_HH
#include "EdgeModel.hh"

EdgeModelPtr CreateCylindricalEdgeCouple(RegionPtr);

// Coupling length
template <typename DoubleType>
class CylindricalEdgeCouple : public EdgeModel {
    public:
        void Serialize(std::ostream &) const;

    private:
        friend class dsModelFactory<CylindricalEdgeCouple>;
        explicit CylindricalEdgeCouple(RegionPtr);

        CylindricalEdgeCouple();
        CylindricalEdgeCouple(const CylindricalEdgeCouple &);
        CylindricalEdgeCouple &operator=(const CylindricalEdgeCouple &);
        void calcCylindricalEdgeCouple2d() const;
        void calcCylindricalEdgeCouple3d() const;
        void calcEdgeScalarValues() const;
};
#endif

