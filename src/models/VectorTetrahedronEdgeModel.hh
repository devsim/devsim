/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef VECTOR_TETRAHEDRON_EDGE_FROM_EDGE_MODEL_HH
#define VECTOR_TETRAHEDRON_EDGE_FROM_EDGE_MODEL_HH
#include "TetrahedronEdgeModel.hh"
TetrahedronEdgeModelPtr CreateVectorTetrahedronEdgeModel(const std::string &, RegionPtr);

template <typename DoubleType>
class VectorTetrahedronEdgeModel : public TetrahedronEdgeModel {
    public:

        void Serialize(std::ostream &) const;

        //// Out naming convention is that the name given is the edge model
        //// The element edge model is edgemodel_ex, edgemodel_ey
        VectorTetrahedronEdgeModel(const std::string &, RegionPtr);

    private:

        void calcTetrahedronEdgeScalarValues() const;

        const std::string elementEdgeModelName;
        std::string       y_ModelName;
        std::string       z_ModelName;
};
#endif
