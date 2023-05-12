/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef EDGECOUPLE_HH
#define EDGECOUPLE_HH
#include "EdgeModel.hh"
// Coupling length
template <typename DoubleType>
class EdgeCouple : public EdgeModel {
    public:
        EdgeCouple(RegionPtr);

        void Serialize(std::ostream &) const;

    private:
        EdgeCouple();
        EdgeCouple(const EdgeCouple &);
        EdgeCouple &operator=(const EdgeCouple &);
        void calcEdgeCouple2d() const;
        void calcEdgeCouple3d() const;
        void calcEdgeScalarValues() const;
};
#endif

