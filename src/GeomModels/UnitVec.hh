/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef UNITVEC_HH
#define UNITVEC_HH
#include "EdgeModel.hh"
template <typename T>
class Vector;
// Coupling length
template <typename DoubleType>
class UnitVec : public EdgeModel {
    public:
        ~UnitVec();

        void Serialize(std::ostream &) const;

    private:
        friend class dsModelFactory<UnitVec>;
        explicit UnitVec(RegionPtr);

        void derived_init();

        UnitVec();
        UnitVec(const UnitVec &);
        UnitVec &operator=(const UnitVec &);
        Vector<DoubleType> calcUnitVec(ConstEdgePtr) const;
        void calcEdgeScalarValues() const;

        WeakEdgeModelPtr unity;
        WeakEdgeModelPtr unitz;
};
#endif
