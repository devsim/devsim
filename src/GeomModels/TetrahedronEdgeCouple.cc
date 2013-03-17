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

#include "TetrahedronEdgeCouple.hh"
#include "Region.hh"
#include "dsAssert.hh"
#include "Vector.hh"
#include "Tetrahedron.hh"
#include "Edge.hh"
#include "Node.hh"
#include "EdgeData.hh"
#include "Triangle.hh"

TetrahedronEdgeCouple::TetrahedronEdgeCouple(RegionPtr rp) :
TetrahedronEdgeModel("ElementEdgeCouple", rp, TetrahedronEdgeModel::SCALAR)
{
}


void TetrahedronEdgeCouple::calcTetrahedronEdgeScalarValues() const
{
  const size_t dimension=GetRegion().GetDimension();

  dsAssert(dimension == 3, "UNEXPECTED");

  calcTetrahedronEdgeCouple();
}

void TetrahedronEdgeCouple::calcTetrahedronEdgeCouple() const
{
  const std::vector<Vector> tetrahedronCenters = GetRegion().GetTetrahedronCenters();
  const std::vector<Vector> triangleCenters    = GetRegion().GetTriangleCenters();
  const Region::TetrahedronToConstEdgeDataList_t &ttelist = GetRegion().GetTetrahedronToEdgeDataList();

  const ConstTetrahedronList &tetrahedronList = GetRegion().GetTetrahedronList();

  const ConstEdgeList &edgeList = GetRegion().GetEdgeList();

  const Region::TetrahedronToConstTriangleList_t &tetrahedronToTriangleList = GetRegion().GetTetrahedronToTriangleList();

  //// Make this part of region, if useful
  std::vector<Vector> edgeCenters(edgeList.size());
  for (size_t i = 0; i < edgeList.size(); ++i)
  { 
      const Edge &edge = *(edgeList[i]);
      Vector edgeCenter = edge.GetHead()->Position();
      edgeCenter += edge.GetTail()->Position();
      edgeCenter *= 0.5;
      edgeCenters[i] = edgeCenter;
  }


  Vector faceToTetCenter[4];

  std::vector<double> ev(6*tetrahedronList.size());

  for (size_t ti = 0; ti < tetrahedronList.size(); ++ti)
  {
    const Vector &tetrahedronCenter = tetrahedronCenters[ti];

    const ConstTriangleList &triangleList = tetrahedronToTriangleList[ti];
    /// A tet only has 4 triangles
    for (size_t i = 0; i < 4; ++i)
    {
      Vector v1 = triangleCenters[triangleList[i]->GetIndex()];
      v1 -= tetrahedronCenter;
      faceToTetCenter[i] = v1;
    }

    const ConstEdgeDataList &edgeDataList = ttelist[ti];
    for (size_t ei = 0; ei < 6; ++ei)
    {
      const EdgeData &edgedata = *(edgeDataList[ei]);

//      const Edge &edge = *(edgedata.edge);

//      const Vector &edgeCenter = edgeCenters[edge.GetIndex()];

      Vector v0 = tetrahedronCenter;
      v0 -= edgeCenters[edgedata.edge->GetIndex()];


      /// The edgeCouple is from center of Tetrahedron to each triangle side to the tetrahedron edge index
      const Vector &v1 = faceToTetCenter[edgedata.triangle_index[0]];
      const Vector &v2 = faceToTetCenter[edgedata.triangle_index[1]];
      ev[6*ti + ei] = 0.5 * (magnitude(cross_prod(v0, v1)) + magnitude(cross_prod(v0, v2)));
    }
  }

  SetValues(ev);
}
void TetrahedronEdgeCouple::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

