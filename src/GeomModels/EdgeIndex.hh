/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef EDGEINDEX_HH
#define EDGEINDEX_HH
#include "EdgeModel.hh"
// Coupling length
template <typename DoubleType>
class EdgeIndex : public EdgeModel {
    public:
        void Serialize(std::ostream &) const;

    private:
        friend class dsModelFactory<EdgeIndex>;
        explicit EdgeIndex(RegionPtr);

        void derived_init();

        EdgeIndex();
        EdgeIndex(const EdgeIndex &);
        EdgeIndex &operator=(const EdgeIndex &);
        DoubleType calcEdgeIndex(ConstEdgePtr) const;
        void calcEdgeScalarValues() const;
};
#endif

