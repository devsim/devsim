/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef TRIANGLE_EDGE_COUPLE_HH
#define TRIANGLE_EDGE_COUPLE_HH

#include "TriangleEdgeModel.hh"

template <typename T>
class Vector;

// Coupling length
template <typename DoubleType>
class TriangleEdgeCouple : public TriangleEdgeModel {
    public:
        void Serialize(std::ostream &) const;

    private:
        friend class dsModelFactory<TriangleEdgeCouple>;
        explicit TriangleEdgeCouple(RegionPtr);

        TriangleEdgeCouple();
        TriangleEdgeCouple(const TriangleEdgeCouple &);
        TriangleEdgeCouple &operator=(const TriangleEdgeCouple &);
        Vector<DoubleType> calcTriangleEdgeCouple(ConstTrianglePtr) const;
        void calcTriangleEdgeScalarValues() const;
};
#endif
