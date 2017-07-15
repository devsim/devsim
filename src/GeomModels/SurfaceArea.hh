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

#ifndef SURFACE_AREA_HH
#define SURFACE_AREA_HH
#include "NodeModel.hh"

class Node;
typedef Node *NodePtr;
typedef const Node *ConstNodePtr;

template <typename DoubleType>
class SurfaceArea : public NodeModel
{
    public:
      SurfaceArea(RegionPtr);

      void Serialize(std::ostream &) const;

    private:
      void calcSurfaceArea3d() const;
      void calcSurfaceArea2d() const;
      void calcSurfaceArea1d() const;
      void calcNodeScalarValues() const;
      void setInitialValues();

      WeakNodeModelPtr nsurf_x;
      WeakNodeModelPtr nsurf_y;
      WeakNodeModelPtr nsurf_z;
};

#endif
