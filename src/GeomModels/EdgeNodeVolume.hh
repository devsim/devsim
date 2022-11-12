/***
DEVSIM
Copyright 2021 DEVSIM LLC

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

#ifndef EDGENODEVOLUME_HH
#define EDGENODEVOLUME_HH
#include "EdgeModel.hh"
// Coupling length
template <typename DoubleType>
class EdgeNodeVolume : public EdgeModel {
    public:
        EdgeNodeVolume(RegionPtr);

        void Serialize(std::ostream &) const;

    private:
        EdgeNodeVolume();
        EdgeNodeVolume(const EdgeNodeVolume &);
        EdgeNodeVolume &operator=(const EdgeNodeVolume &);
        void calcEdgeNodeVolume1d() const;
        void calcEdgeNodeVolume2d() const;
        void calcEdgeNodeVolume3d() const;
        void calcEdgeScalarValues() const;
};
#endif

