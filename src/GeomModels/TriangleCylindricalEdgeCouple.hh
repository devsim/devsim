/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef TRIANGLE_CYLINDRICAL_EDGE_COUPLE_HH
#define TRIANGLE_CYLINDRICAL_EDGE_COUPLE_HH

#include "TriangleEdgeModel.hh"

template <typename T>
class Vector;


TriangleEdgeModelPtr CreateTriangleCylindricalEdgeCouple(RegionPtr);

// Coupling length
template <typename DoubleType>
class TriangleCylindricalEdgeCouple : public TriangleEdgeModel {
    public:

        void Serialize(std::ostream &) const;

        TriangleCylindricalEdgeCouple(RegionPtr);

    private:

        TriangleCylindricalEdgeCouple();
        TriangleCylindricalEdgeCouple(const TriangleCylindricalEdgeCouple &);
        TriangleCylindricalEdgeCouple &operator=(const TriangleCylindricalEdgeCouple &);
        Vector<DoubleType> calcTriangleCylindricalEdgeCouple(ConstTrianglePtr, const std::string &/*RAxisVariable*/, DoubleType /*RAxis0*/) const;
        void calcTriangleEdgeScalarValues() const;
};
#endif

