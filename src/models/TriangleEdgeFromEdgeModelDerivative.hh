/***
DEVSIM
Copyright 2013 Devsim LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
***/

#ifndef TRIANGLE_EDGE_FROM_EDGE_MODEL_DERIVATIVE_HH
#define TRIANGLE_EDGE_FROM_EDGE_MODEL_DERIVATIVE_HH
#include "TriangleEdgeModel.hh"
template <typename DoubleType>
class TriangleEdgeFromEdgeModelDerivative : public TriangleEdgeModel {
    public:
        //// Out naming convention is that the name given is the edge model
        //// The element edge model is edgemodel_ex, edgemodel_ey
        TriangleEdgeFromEdgeModelDerivative(
          const std::string &/*edgemodel*/,
          const std::string &/*nodemodel*/,
          RegionPtr /*rp*/
        );

        void Serialize(std::ostream &) const;

    private:
        void calcTriangleEdgeScalarValues() const;

        const std::string edgeModelName;
        const std::string nodeModelName;

        // Detect whether parent model still exists
        std::string edgeModelName0;
        std::string edgeModelName1;
        std::string x_ModelName1;
        std::string x_ModelName2;
        std::string y_ModelName0;
        std::string y_ModelName1;
        std::string y_ModelName2;
};
#endif
