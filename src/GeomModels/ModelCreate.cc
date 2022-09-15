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
#include "AtContactNode.hh"
#include "EdgeCouple.hh"
#include "EdgeIndex.hh"
#include "EdgeInverseLength.hh"
#include "EdgeLength.hh"
#include "EdgeNodeVolume.hh"
#include "GlobalData.hh"
#include "NodePosition.hh"
#include "NodeVolume.hh"
#include "Region.hh"
#include "SurfaceArea.hh"
#include "TetrahedronEdgeCouple.hh"
#include "TetrahedronNodeVolume.hh"
#include "TriangleEdgeCouple.hh"
#include "TriangleNodeVolume.hh"
#include "UnitVec.hh"

#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
#endif

/**
 * This creates the base models used by all of the other models
 */

namespace {
template <typename DoubleType> void CreateDefaultModelsImpl(RegionPtr rp) {
  new EdgeCouple<DoubleType>(rp);
  new EdgeLength<DoubleType>(rp);
  new EdgeIndex<DoubleType>(rp);
  new EdgeNodeVolume<DoubleType>(rp);
  new NodeVolume<DoubleType>(rp);
  new EdgeInverseLength<DoubleType>(rp);
  new NodePosition<DoubleType>(rp);
  new AtContactNode<DoubleType>(rp);
  new SurfaceArea<DoubleType>(rp);
  new UnitVec<DoubleType>(rp);

  const size_t dimension = rp->GetDimension();

  if (dimension == 2) {
    new TriangleEdgeCouple<DoubleType>(rp);
    new TriangleNodeVolume<DoubleType>(rp);
  } else if (dimension == 3) {
    new TetrahedronEdgeCouple<DoubleType>(rp);
    new TetrahedronNodeVolume<DoubleType>(rp);
  }
}
} // namespace

void CreateDefaultModels(RegionPtr rp) {
#ifdef DEVSIM_EXTENDED_PRECISION
  CreateDefaultModelsImpl<float128>(rp);
#else
  CreateDefaultModelsImpl<double>(rp);
#endif
}
