/***
DEVSIM
Copyright 2021 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef EDGENODEVOLUME_HH
#define EDGENODEVOLUME_HH
#include "EdgeModel.hh"
// Coupling length
template <typename DoubleType>
class EdgeNodeVolume : public EdgeModel {
    public:
        EdgeNodeVolume(RegionPtr);

        void Serialize(std::ostream &) const;

    private:
        EdgeNodeVolume();
        EdgeNodeVolume(const EdgeNodeVolume &);
        EdgeNodeVolume &operator=(const EdgeNodeVolume &);
        void calcEdgeNodeVolume1d() const;
        void calcEdgeNodeVolume2d() const;
        void calcEdgeNodeVolume3d() const;
        void calcEdgeScalarValues() const;
};
#endif

