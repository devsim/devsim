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

#ifndef CYLINDRICALEDGECOUPLE_HH
#define CYLINDRICALEDGECOUPLE_HH
#include "EdgeModel.hh"
// Coupling length
class CylindricalEdgeCouple : public EdgeModel {
    public:
        CylindricalEdgeCouple(RegionPtr);

        void Serialize(std::ostream &) const;

    private:
        CylindricalEdgeCouple();
        CylindricalEdgeCouple(const CylindricalEdgeCouple &);
        CylindricalEdgeCouple &operator=(const CylindricalEdgeCouple &);
        void calcCylindricalEdgeCouple2d() const;
        void calcCylindricalEdgeCouple3d() const;
        void calcEdgeScalarValues() const;
};
#endif
