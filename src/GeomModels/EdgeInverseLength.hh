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
        EdgeInverseLength(RegionPtr);

        void Serialize(std::ostream &) const;

    private:
        EdgeInverseLength();
        EdgeInverseLength(const EdgeInverseLength &);
        EdgeInverseLength &operator=(const EdgeInverseLength &);
        void calcEdgeScalarValues() const;
};
#endif

