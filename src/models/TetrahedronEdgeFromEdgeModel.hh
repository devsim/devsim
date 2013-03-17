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

#ifndef TETRAHEDRON_EDGE_FROM_EDGE_MODEL_HH
#define TETRAHEDRON_EDGE_FROM_EDGE_MODEL_HH
#include "TetrahedronEdgeModel.hh"
class TetrahedronEdgeFromEdgeModel : public TetrahedronEdgeModel {
    public:
        //// Out naming convention is that the name given is the edge model
        //// The element edge model is edgemodel_ex, edgemodel_ey
        TetrahedronEdgeFromEdgeModel(const std::string &, RegionPtr);

        void Serialize(std::ostream &) const;

    private:
        void calcTetrahedronEdgeScalarValues() const;

        const std::string edgeModelName;
        std::string       y_ModelName;
        std::string       z_ModelName;
};
#endif
