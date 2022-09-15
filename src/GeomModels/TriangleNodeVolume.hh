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

#ifndef TRIANGLE_NODE_VOLUME_HH
#define TRIANGLE_NODE_VOLUME_HH
#include "TriangleEdgeModel.hh"

template <typename DoubleType>
class TriangleNodeVolume : public TriangleEdgeModel {
public:
  TriangleNodeVolume(RegionPtr);

  void Serialize(std::ostream &) const;

private:
  TriangleNodeVolume();
  TriangleNodeVolume(const TriangleNodeVolume &);
  TriangleNodeVolume &operator=(const TriangleNodeVolume &);
  void calcTriangleEdgeScalarValues() const;
  void setInitialValues();
};

#endif
