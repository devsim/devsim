/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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

template <typename DoubleType>
TetrahedronEdgeCouple<DoubleType>::TetrahedronEdgeCouple(RegionPtr rp) :
TetrahedronEdgeModel("ElementEdgeCouple", rp, TetrahedronEdgeModel::DisplayType::SCALAR)
{
}


template <typename DoubleType>
void TetrahedronEdgeCouple<DoubleType>::calcTetrahedronEdgeScalarValues() const
{
  const size_t dimension=GetRegion().GetDimension();

  dsAssert(dimension == 3, "UNEXPECTED");

  calcTetrahedronEdgeCouple();
}

template <typename DoubleType>
void TetrahedronEdgeCouple<DoubleType>::calcTetrahedronEdgeCouple() const
{
  const std::vector<Vector<DoubleType>> tetrahedronCenters = GetRegion().template GetTetrahedronCenters<DoubleType>();
  const std::vector<Vector<DoubleType>> triangleCenters    = GetRegion().template GetTriangleCenters<DoubleType>();
  const Region::TetrahedronToConstEdgeDataList_t &ttelist = GetRegion().GetTetrahedronToEdgeDataList();

  const ConstTetrahedronList &tetrahedronList = GetRegion().GetTetrahedronList();

  const ConstEdgeList &edgeList = GetRegion().GetEdgeList();

  const Region::TetrahedronToConstTriangleList_t &tetrahedronToTriangleList = GetRegion().GetTetrahedronToTriangleList();

  //// Make this part of region, if useful
  std::vector<Vector<DoubleType>> edgeCenters(edgeList.size());
  for (size_t i = 0; i < edgeList.size(); ++i)
  {
      const Edge &edge = *(edgeList[i]);
      const auto &h0 = edge.GetHead()->Position();
      const auto &h1 = edge.GetTail()->Position();
      Vector<DoubleType> edgeCenter(h0.Getx(), h0.Gety(), h0.Getz());
      edgeCenter += Vector<DoubleType>(h1.Getx(), h1.Gety(), h1.Getz());
      edgeCenter *= 0.5;
      edgeCenters[i] = edgeCenter;
  }


  Vector<DoubleType> faceToTetCenter[4];

  std::vector<DoubleType> ev(6*tetrahedronList.size());

  for (size_t ti = 0; ti < tetrahedronList.size(); ++ti)
  {
    const Vector<DoubleType> &tetrahedronCenter = tetrahedronCenters[ti];

    const ConstTriangleList &triangleList = tetrahedronToTriangleList[ti];
    /// A tet only has 4 triangles
    for (size_t i = 0; i < 4; ++i)
    {
      Vector<DoubleType> v1 = triangleCenters[triangleList[i]->GetIndex()];
      v1 -= tetrahedronCenter;
      faceToTetCenter[i] = v1;
    }

    const ConstEdgeDataList &edgeDataList = ttelist[ti];
    for (size_t ei = 0; ei < 6; ++ei)
    {
      const EdgeData &edgedata = *(edgeDataList[ei]);

//      const Edge &edge = *(edgedata.edge);

//      const Vector<DoubleType> &edgeCenter = edgeCenters[edge.GetIndex()];

      Vector<DoubleType> v0 = tetrahedronCenter;
      v0 -= edgeCenters[edgedata.edge->GetIndex()];


      /// The edgeCouple is from center of Tetrahedron to each triangle side to the tetrahedron edge index
      const Vector<DoubleType> &v1 = faceToTetCenter[edgedata.triangle_index[0]];
      const Vector<DoubleType> &v2 = faceToTetCenter[edgedata.triangle_index[1]];
      ev[6*ti + ei] = 0.5 * (magnitude(cross_prod(v0, v1)) + magnitude(cross_prod(v0, v2)));
    }
  }

  SetValues(ev);
}
template <typename DoubleType>
void TetrahedronEdgeCouple<DoubleType>::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

template class TetrahedronEdgeCouple<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class TetrahedronEdgeCouple<float128>;
#endif

