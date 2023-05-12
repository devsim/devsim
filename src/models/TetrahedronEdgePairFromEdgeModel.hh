/***
DEVSIM
Copyright 2020 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef TETRAHEDRON_EDGE_PAIR_FROM_EDGE_MODEL_HH
#define TETRAHEDRON_EDGE_PAIR_FROM_EDGE_MODEL_HH
#include "TetrahedronEdgeModel.hh"
#include <string>
#include <array>

TetrahedronEdgeModelPtr CreateTetrahedronEdgePairFromEdgeModel(const std::string &, RegionPtr);

template <typename DoubleType>
class TetrahedronEdgePairFromEdgeModel : public TetrahedronEdgeModel {
    public:

        void Serialize(std::ostream &) const;

        TetrahedronEdgePairFromEdgeModel(const std::string &, RegionPtr);


    private:

        void calcTetrahedronEdgeScalarValues() const;

        const std::string edgeModelName;
        std::array<std::array<std::string, 3>, 2> model_names;
};
#endif

