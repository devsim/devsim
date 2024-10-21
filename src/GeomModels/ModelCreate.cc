/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "ModelCreate.hh"
#include "ModelFactory.hh"
#include "NodeVolume.hh"
#include "EdgeNodeVolume.hh"
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
#include "GlobalData.hh"

#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
#endif

/**
 * This creates the base models used by all of the other models
 */

namespace {
template <typename DoubleType>
void CreateDefaultModelsImpl(RegionPtr rp)
{
  dsModelFactory<EdgeCouple<DoubleType>>::create(rp);
  dsModelFactory<EdgeLength<DoubleType>>::create(rp);
  dsModelFactory<EdgeIndex<DoubleType>>::create(rp);
  dsModelFactory<EdgeNodeVolume<DoubleType>>::create(rp);
  dsModelFactory<NodeVolume<DoubleType>>::create(rp);
  dsModelFactory<EdgeInverseLength<DoubleType>>::create(rp);
  dsModelFactory<NodePosition<DoubleType>>::create(rp);
  dsModelFactory<AtContactNode<DoubleType>>::create(rp);
  dsModelFactory<SurfaceArea<DoubleType>>::create(rp);
  dsModelFactory<UnitVec<DoubleType>>::create(rp);

  const size_t dimension = rp->GetDimension();

  if (dimension == 2)
  {
    dsModelFactory<TriangleEdgeCouple<DoubleType>>::create(rp);
    dsModelFactory<TriangleNodeVolume<DoubleType>>::create(rp);
  }
  else if (dimension == 3)
  {
    dsModelFactory<TetrahedronEdgeCouple<DoubleType>>::create(rp);
    dsModelFactory<TetrahedronNodeVolume<DoubleType>>::create(rp);
  }
}
}

void CreateDefaultModels(RegionPtr rp)
{
#ifdef DEVSIM_EXTENDED_PRECISION
  CreateDefaultModelsImpl<float128>(rp);
#else
  CreateDefaultModelsImpl<double>(rp);
#endif
}

