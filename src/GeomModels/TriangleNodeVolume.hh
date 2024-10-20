/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef TRIANGLE_NODE_VOLUME_HH
#define TRIANGLE_NODE_VOLUME_HH
#include "TriangleEdgeModel.hh"

template <typename DoubleType>
class TriangleNodeVolume : public TriangleEdgeModel
{
    public:
        void Serialize(std::ostream &) const;

    private:
        friend class dsModelFactory<TriangleNodeVolume>;
        explicit TriangleNodeVolume(RegionPtr);

        void derived_init();

        TriangleNodeVolume();
        TriangleNodeVolume(const TriangleNodeVolume &);
        TriangleNodeVolume &operator=(const TriangleNodeVolume &);
        void   calcTriangleEdgeScalarValues() const;
        void   setInitialValues();
};

#endif

