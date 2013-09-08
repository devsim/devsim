/***
DEVSIM
Copyright 2013 Devsim LLC

This file is part of DEVSIM.

DEVSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, version 3.

DEVSIM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with DEVSIM.  If not, see <http://www.gnu.org/licenses/>.
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
    new EdgeCouple(rp);
    new EdgeLength(rp);
    new EdgeIndex(rp);
    new NodeVolume(rp);
    new EdgeInverseLength(rp);
    new NodePosition(rp);
    new AtContactNode(rp);
    new SurfaceArea(rp);
    new UnitVec(rp);

    const size_t dimension = rp->GetDimension();

    if (dimension == 2)
    {
      new TriangleEdgeCouple(rp);
      new TriangleNodeVolume(rp);
    }
    else if (dimension == 3)
    {
      new TetrahedronEdgeCouple(rp);
      new TetrahedronNodeVolume(rp);
    }
}

