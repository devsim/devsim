/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef EDGELENGTH_HH
#define EDGELENGTH_HH
#include "EdgeModel.hh"
// Coupling length
template <typename DoubleType>
class EdgeLength : public EdgeModel {
    public:
        EdgeLength(RegionPtr);

        void Serialize(std::ostream &) const;

    private:
        EdgeLength();
        EdgeLength(const EdgeLength &);
        EdgeLength &operator=(const EdgeLength &);
        DoubleType calcEdgeLength(ConstEdgePtr) const;
        void calcEdgeScalarValues() const;
};
#endif

