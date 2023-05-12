/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "TriangleEdgeCouple.hh"
#include "Region.hh"
#include "dsAssert.hh"
#include "Vector.hh"
#include "Triangle.hh"
#include "Edge.hh"
#include "Node.hh"

template <typename DoubleType>
TriangleEdgeCouple<DoubleType>::TriangleEdgeCouple(RegionPtr rp) :
TriangleEdgeModel("ElementEdgeCouple", rp, TriangleEdgeModel::DisplayType::SCALAR)
{
}


template <typename DoubleType>
void TriangleEdgeCouple<DoubleType>::calcTriangleEdgeScalarValues() const
{
  const size_t dimension=GetRegion().GetDimension();

  dsAssert(dimension == 2, "UNEXPECTED");

  const ConstTriangleList &el = GetRegion().GetTriangleList();
  std::vector<DoubleType> ev(3*el.size());

  for (size_t i = 0; i < el.size(); ++i)
  {
    const Vector<DoubleType> &v = calcTriangleEdgeCouple(el[i]);
    const size_t indexi = 3*i;
    ev[indexi]     = v.Getx();
    ev[indexi + 1] = v.Gety();
    ev[indexi + 2] = v.Getz();
  }

  SetValues(ev);
}

template <typename DoubleType>
Vector<DoubleType> TriangleEdgeCouple<DoubleType>::calcTriangleEdgeCouple(ConstTrianglePtr tp) const
{
    const Triangle &triangle = *tp;
    const std::vector<Vector<DoubleType>> &centers = GetRegion().template GetTriangleCenters<DoubleType>();

    const Region::TriangleToConstEdgeList_t &ttelist = GetRegion().GetTriangleToEdgeList();
    size_t tindex = triangle.GetIndex();
    const ConstEdgeList &edgeList = ttelist[tindex];

    const Vector<DoubleType> &vc = centers[triangle.GetIndex()];

    DoubleType ec[3];

    for (size_t i = 0; i < 3; ++i)
    {
      const Edge &edge = *edgeList[i];

      const auto &h0 = edge.GetHead()->Position();
      const auto &h1 = edge.GetTail()->Position();

      Vector<DoubleType> vm(h0.Getx(), h0.Gety(), h0.Getz());
      vm += Vector<DoubleType>(h1.Getx(), h1.Gety(), h1.Getz());
      vm *= 0.5;
      vm -= vc;
      ec[i] = vm.magnitude();
      // TODO: think about what happens if we are outside the triangle
    }

    return Vector<DoubleType>(ec[0], ec[1], ec[2]);
}

template <typename DoubleType>
void TriangleEdgeCouple<DoubleType>::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

template class TriangleEdgeCouple<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class TriangleEdgeCouple<float128>;
#endif

