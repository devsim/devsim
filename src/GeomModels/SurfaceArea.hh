/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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

      WeakNodeModelPtr contact_area;
      WeakNodeModelPtr nsurf_x;
      WeakNodeModelPtr nsurf_y;
      WeakNodeModelPtr nsurf_z;
};

#endif
