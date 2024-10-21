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
        void Serialize(std::ostream &) const;

    private:
        friend class dsModelFactory<EdgeCouple>;
        explicit EdgeCouple(RegionPtr);

        void derived_init();

        EdgeCouple();
        EdgeCouple(const EdgeCouple &);
        EdgeCouple &operator=(const EdgeCouple &);
        void calcEdgeCouple2d() const;
        void calcEdgeCouple3d() const;
        void calcEdgeScalarValues() const;
};
#endif

