/***
DEVSIM
Copyright 2013 Devsim LLC

This file is part of DEVSIM.

DEVSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, version 3.

DEVSIM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with DEVSIM.  If not, see <http://www.gnu.org/licenses/>.
***/

#ifndef TRIANGLE_CYLINDRICAL_NODE_VOLUME_HH
#define TRIANGLE_CYLINDRICAL_NODE_VOLUME_HH
#include "TriangleEdgeModel.hh"

#include <vector>

class Triangle;
typedef Triangle *TrianglePtr;
typedef const Triangle *ConstTrianglePtr;

class TriangleCylindricalNodeVolume : public TriangleEdgeModel
{
    public:
        TriangleCylindricalNodeVolume(RegionPtr);

        void Serialize(std::ostream &) const;

    private:
        TriangleCylindricalNodeVolume();
        TriangleCylindricalNodeVolume(const TriangleCylindricalNodeVolume &);
        TriangleCylindricalNodeVolume &operator=(const TriangleCylindricalNodeVolume &);
        std::vector<double> calcTriangleCylindricalNodeVolume(ConstTrianglePtr, const std::string &/*RAxisVariable*/, double /*RAxis0*/) const;
        void   calcTriangleEdgeScalarValues() const;
        void   setInitialValues();

        /// this is on the element since node volumes are no longer symmetric
        WeakTriangleEdgeModelPtr node1Volume_;
};
#endif

