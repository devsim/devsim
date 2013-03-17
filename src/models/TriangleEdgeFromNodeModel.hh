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

#ifndef TRIANGLE_EDGE_FROM_NODE_MODEL_HH
#define TRIANGLE_EDGE_FROM_NODE_MODEL_HH
#include "TriangleEdgeModel.hh"
class TriangleEdgeFromNodeModel : public TriangleEdgeModel {
    public:
        //// Out naming convention is that the name given is the edge model
        //// The element edge model is edgemodel_ex, edgemodel_ey
        TriangleEdgeFromNodeModel(const std::string &/*node0*/, const std::string &/*node1*/, const std::string &/*node2*/, const std::string &/*nodemodel*/, RegionPtr);

        void Serialize(std::ostream &) const;

    private:
        void calcTriangleEdgeScalarValues() const;

        // Detect whether parent model still exists
        const std::string nodeModelName;
        const std::string edgeModel1Name;
        const std::string edgeModel2Name;
};
#endif
