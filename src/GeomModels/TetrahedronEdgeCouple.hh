/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef TETRAHEDRON_EDGE_COUPLE_HH
#define TETRAHEDRON_EDGE_COUPLE_HH

#include "TetrahedronEdgeModel.hh"

// Coupling length
template <typename DoubleType>
class TetrahedronEdgeCouple : public TetrahedronEdgeModel {
    public:
        void Serialize(std::ostream &) const;

    private:
        friend class dsModelFactory<TetrahedronEdgeCouple>;
        explicit TetrahedronEdgeCouple(RegionPtr);

        void derived_init();

        TetrahedronEdgeCouple();
        TetrahedronEdgeCouple(const TetrahedronEdgeCouple &);
        TetrahedronEdgeCouple &operator=(const TetrahedronEdgeCouple &);
        void calcTetrahedronEdgeCouple() const;
        void calcTetrahedronEdgeScalarValues() const;
};
#endif
