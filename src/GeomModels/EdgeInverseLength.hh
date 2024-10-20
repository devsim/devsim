/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef EDGEINVERSELENGTH_HH
#define EDGEINVERSELENGTH_HH
#include "EdgeModel.hh"
// Coupling length
template <typename DoubleType>
class EdgeInverseLength : public EdgeModel {
    public:
        void Serialize(std::ostream &) const;

    private:
        friend class dsModelFactory<EdgeInverseLength>;
        explicit EdgeInverseLength(RegionPtr);

        void derived_init();

        EdgeInverseLength();
        EdgeInverseLength(const EdgeInverseLength &);
        EdgeInverseLength &operator=(const EdgeInverseLength &);
        void calcEdgeScalarValues() const;
};
#endif

