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

#ifndef CYLINDRICALEDGENODEVOLUME_HH
#define CYLINDRICALEDGENODEVOLUME_HH
#include "EdgeModel.hh"

EdgeModelPtr CreateCylindricalEdgeNodeVolume(RegionPtr);

template <typename DoubleType>
class CylindricalEdgeNodeVolume : public EdgeModel
{
    public:

      void Serialize(std::ostream &) const;

      CylindricalEdgeNodeVolume(RegionPtr);

    private:

      void calcEdgeScalarValues() const;

      WeakEdgeModelPtr node1Volume_;
};
#endif

