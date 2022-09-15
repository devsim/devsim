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

#ifndef TRIANGLE_CYLINDRICAL_NODE_VOLUME_HH
#define TRIANGLE_CYLINDRICAL_NODE_VOLUME_HH
#include "TriangleEdgeModel.hh"

#include <vector>

class Triangle;
typedef Triangle *TrianglePtr;
typedef const Triangle *ConstTrianglePtr;

TriangleEdgeModelPtr CreateTriangleCylindricalNodeVolume(RegionPtr);

template <typename DoubleType>
class TriangleCylindricalNodeVolume : public TriangleEdgeModel {
public:
  void Serialize(std::ostream &) const;

  TriangleCylindricalNodeVolume(RegionPtr);

private:
  TriangleCylindricalNodeVolume();
  TriangleCylindricalNodeVolume(const TriangleCylindricalNodeVolume &);
  TriangleCylindricalNodeVolume &
  operator=(const TriangleCylindricalNodeVolume &);
  std::vector<DoubleType>
  calcTriangleCylindricalNodeVolume(ConstTrianglePtr,
                                    const std::string & /*RAxisVariable*/,
                                    DoubleType /*RAxis0*/) const;
  void calcTriangleEdgeScalarValues() const;
  void setInitialValues();

  /// this is on the element since node volumes are no longer symmetric
  WeakTriangleEdgeModelPtr node1Volume_;
};
#endif
