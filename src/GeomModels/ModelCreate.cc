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

#include "ModelCreate.hh"
#include "NodeVolume.hh"
#include "EdgeCouple.hh"
#include "EdgeLength.hh"
#include "EdgeIndex.hh"
#include "EdgeInverseLength.hh"
#include "NodePosition.hh"
#include "AtContactNode.hh"
#include "SurfaceArea.hh"
#include "UnitVec.hh"
#include "TriangleEdgeCouple.hh"
#include "TriangleNodeVolume.hh"
#include "TetrahedronEdgeCouple.hh"
#include "TetrahedronNodeVolume.hh"
#include "Region.hh"

/**
 * This creates the base models used by all of the other models
 */
void CreateDefaultModels(RegionPtr rp)
{
    new EdgeCouple<double>(rp);
    new EdgeLength<double>(rp);
    new EdgeIndex<double>(rp);
    new NodeVolume<double>(rp);
    new EdgeInverseLength<double>(rp);
    new NodePosition<double>(rp);
    new AtContactNode<double>(rp);
    new SurfaceArea<double>(rp);
    new UnitVec<double>(rp);

    const size_t dimension = rp->GetDimension();

    if (dimension == 2)
    {
      new TriangleEdgeCouple<double>(rp);
      new TriangleNodeVolume<double>(rp);
    }
    else if (dimension == 3)
    {
      new TetrahedronEdgeCouple<double>(rp);
      new TetrahedronNodeVolume<double>(rp);
    }
}

