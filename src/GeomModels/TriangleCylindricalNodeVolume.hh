/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef TRIANGLE_CYLINDRICAL_NODE_VOLUME_HH
#define TRIANGLE_CYLINDRICAL_NODE_VOLUME_HH
#include "TriangleEdgeModel.hh"

#include <vector>

class Triangle;
typedef Triangle *TrianglePtr;
typedef const Triangle *ConstTrianglePtr;

TriangleEdgeModelPtr CreateTriangleCylindricalNodeVolume(RegionPtr);

template <typename DoubleType>
class TriangleCylindricalNodeVolume : public TriangleEdgeModel
{
    public:

        void Serialize(std::ostream &) const;

        TriangleCylindricalNodeVolume(RegionPtr);

    private:

        TriangleCylindricalNodeVolume();
        TriangleCylindricalNodeVolume(const TriangleCylindricalNodeVolume &);
        TriangleCylindricalNodeVolume &operator=(const TriangleCylindricalNodeVolume &);
        std::vector<DoubleType> calcTriangleCylindricalNodeVolume(ConstTrianglePtr, const std::string &/*RAxisVariable*/, DoubleType /*RAxis0*/) const;
        void   calcTriangleEdgeScalarValues() const;
        void   setInitialValues();

        /// this is on the element since node volumes are no longer symmetric
        WeakTriangleEdgeModelPtr node1Volume_;
};
#endif

