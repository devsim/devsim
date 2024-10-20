/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef TETRAHEDRON_EDGE_FROM_EDGE_MODEL_HH
#define TETRAHEDRON_EDGE_FROM_EDGE_MODEL_HH
#include "TetrahedronEdgeModel.hh"

TetrahedronEdgeModelPtr CreateTetrahedronEdgeFromEdgeModel(const std::string &, RegionPtr);

template <typename DoubleType>
class TetrahedronEdgeFromEdgeModel : public TetrahedronEdgeModel {
    public:
        void Serialize(std::ostream &) const;

    private:
        friend class dsModelFactory<TetrahedronEdgeFromEdgeModel>;
        TetrahedronEdgeFromEdgeModel(const std::string &, RegionPtr);

        void derived_init();

        void calcTetrahedronEdgeScalarValues() const;

        const std::string edgeModelName;
        std::string       y_ModelName;
        std::string       z_ModelName;
};
#endif
