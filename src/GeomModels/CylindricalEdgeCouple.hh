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

#ifndef CYLINDRICALEDGECOUPLE_HH
#define CYLINDRICALEDGECOUPLE_HH
#include "EdgeModel.hh"

EdgeModelPtr CreateCylindricalEdgeCouple(RegionPtr);

// Coupling length
template <typename DoubleType>
class CylindricalEdgeCouple : public EdgeModel {
    public:
        CylindricalEdgeCouple(RegionPtr);

        void Serialize(std::ostream &) const;

    private:
        CylindricalEdgeCouple();
        CylindricalEdgeCouple(const CylindricalEdgeCouple &);
        CylindricalEdgeCouple &operator=(const CylindricalEdgeCouple &);
        void calcCylindricalEdgeCouple2d() const;
        void calcCylindricalEdgeCouple3d() const;
        void calcEdgeScalarValues() const;
};
#endif

