/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef TETRAHEDRON_EDGE_SUB_MODEL_HH
#define TETRAHEDRON_EDGE_SUB_MODEL_HH
#include "TetrahedronEdgeModel.hh"
#include <string>

TetrahedronEdgeModelPtr CreateTetrahedronEdgeSubModel(const std::string &, RegionPtr, TetrahedronEdgeModel::DisplayType);
TetrahedronEdgeModelPtr CreateTetrahedronEdgeSubModel(const std::string &, RegionPtr, TetrahedronEdgeModel::DisplayType, TetrahedronEdgeModelPtr);

template <typename DoubleType>
class TetrahedronEdgeSubModel : public TetrahedronEdgeModel
{
    public:
        TetrahedronEdgeSubModel(const std::string &, RegionPtr, TetrahedronEdgeModel::DisplayType dt);
        // This model depends on this model to calculate values
        TetrahedronEdgeSubModel(const std::string &, RegionPtr, TetrahedronEdgeModel::DisplayType dt, ConstTetrahedronEdgeModelPtr);

        void Serialize(std::ostream &) const;

        static TetrahedronEdgeModelPtr CreateTetrahedronEdgeSubModel(const std::string &, RegionPtr, TetrahedronEdgeModel::DisplayType);
        static TetrahedronEdgeModelPtr CreateTetrahedronEdgeSubModel(const std::string &, RegionPtr, TetrahedronEdgeModel::DisplayType, ConstTetrahedronEdgeModelPtr);

    private:
        void calcTetrahedronEdgeScalarValues() const;
        // If we are an auxilary model, create our values from the parent
        mutable WeakConstTetrahedronEdgeModelPtr parentModel;
        // Detect whether parent model still exists
        mutable std::string parentModelName;
};

#endif
