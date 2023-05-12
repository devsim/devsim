/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef TETRAHEDRON_EDGE_FROM_EDGE_MODEL_DERIVATIVE_HH
#define TETRAHEDRON_EDGE_FROM_EDGE_MODEL_DERIVATIVE_HH
#include "TetrahedronEdgeModel.hh"

TetrahedronEdgeModelPtr CreateTetrahedronEdgeFromEdgeModelDerivative(
  const std::string &/*edgemodel*/,
  const std::string &/*nodemodel*/,
  RegionPtr /*rp*/
);

template <typename DoubleType>
class TetrahedronEdgeFromEdgeModelDerivative : public TetrahedronEdgeModel {
    public:

        void Serialize(std::ostream &) const;

        //// Out naming convention is that the name given is the edge model
        //// The element edge model is edgemodel_ex, edgemodel_ey
        TetrahedronEdgeFromEdgeModelDerivative(
          const std::string &/*edgemodel*/,
          const std::string &/*nodemodel*/,
          RegionPtr /*rp*/
        );

    private:
        void calcTetrahedronEdgeScalarValues() const;


        const std::string edgeModelName;
        const std::string nodeModelName;

        // Detect whether parent model still exists
        std::string edgeModelName0;
        std::string edgeModelName1;
        std::string x_ModelName1;
        std::string x_ModelName2;
        std::string x_ModelName3;
        std::string y_ModelName0;
        std::string y_ModelName1;
        std::string y_ModelName2;
        std::string y_ModelName3;
        std::string z_ModelName0;
        std::string z_ModelName1;
        std::string z_ModelName2;
        std::string z_ModelName3;
};
#endif

