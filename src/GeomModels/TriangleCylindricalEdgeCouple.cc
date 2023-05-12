/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "TriangleCylindricalEdgeCouple.hh"
#include "Region.hh"
#include "dsAssert.hh"
#include "Vector.hh"
#include "Triangle.hh"
#include "Edge.hh"
#include "Node.hh"
#include "GlobalData.hh"
#include "GeometryStream.hh"
#include "ObjectHolder.hh"
#include <cmath>
using std::abs;

template <typename DoubleType>
TriangleCylindricalEdgeCouple<DoubleType>::TriangleCylindricalEdgeCouple(RegionPtr rp) :
TriangleEdgeModel("ElementCylindricalEdgeCouple", rp, TriangleEdgeModel::DisplayType::SCALAR)
{
  RegisterCallback("raxis_zero");
  RegisterCallback("raxis_variable");
}


template <typename DoubleType>
void TriangleCylindricalEdgeCouple<DoubleType>::calcTriangleEdgeScalarValues() const
{
  const Region &region = GetRegion();

  const size_t dimension=region.GetDimension();

  dsAssert(dimension == 2, "UNEXPECTED");

  DoubleType RAxis0 = 0.0;
  std::string RAxisVariable;

  {
    //// TODO: Make this common to all cylindrical code, right now this is a copy and paste
    GlobalData &ginst = GlobalData::GetInstance();

    std::ostringstream os;

    GlobalData::DoubleDBEntry_t raxisdbentry = ginst.GetDoubleDBEntryOnRegion(&region, "raxis_zero");
    if (raxisdbentry.first)
    {
      RAxis0 = raxisdbentry.second;
    }
    else
    {
      os << "raxis_zero on Device " << region.GetDeviceName() << " on Region " << region.GetName() << " must be a valid number parameter\n";
    }

    GlobalData::DBEntry_t zvardbentry = ginst.GetDBEntryOnRegion(&region, "raxis_variable");
    if (!zvardbentry.first)
    {
      os << "raxis_variable on Device " << region.GetDeviceName() << " on Region " << region.GetName() << " must be a valid parameter\n";
    }
    else
    {
      RAxisVariable = zvardbentry.second.GetString();
      if ((RAxisVariable != "x") && (RAxisVariable != "y"))
      {
        os << "raxis_variable on Device " << region.GetDeviceName() << " on Region " << region.GetName() << " must be \"x\" or \"y\"\n";
      }
    }

    if (!os.str().empty())
    {
      GeometryStream::WriteOut(OutputStream::OutputType::FATAL, GetRegion(), os.str());
    }
  }

  const ConstTriangleList &el = region.GetTriangleList();
  std::vector<DoubleType> ev(3*el.size());

  for (size_t i = 0; i < el.size(); ++i)
  {
    const Vector<DoubleType> &v = calcTriangleCylindricalEdgeCouple(el[i], RAxisVariable, RAxis0);
    const size_t indexi = 3*i;
    ev[indexi]     = v.Getx();
    ev[indexi + 1] = v.Gety();
    ev[indexi + 2] = v.Getz();
  }

  SetValues(ev);
}

template <typename DoubleType>
Vector<DoubleType> TriangleCylindricalEdgeCouple<DoubleType>::calcTriangleCylindricalEdgeCouple(ConstTrianglePtr tp, const std::string &RAxisVariable, DoubleType RAxis0) const
{
  const Region &region = GetRegion();

  const Triangle &triangle = *tp;
  const std::vector<Vector<DoubleType>> &centers = region.GetTriangleCenters<DoubleType>();

  const Region::TriangleToConstEdgeList_t &ttelist = region.GetTriangleToEdgeList();
  size_t tindex = triangle.GetIndex();
  const ConstEdgeList &edgeList = ttelist[tindex];

  const Vector<DoubleType> &vc = centers[triangle.GetIndex()];

  DoubleType ec[3];

  DoubleType r0 = 0.0;
  DoubleType r1 = 0.0;


  if (RAxisVariable == "x")
  {
    r1 = vc.Getx() - RAxis0;
  }
  else if (RAxisVariable == "y")
  {
    r1 = vc.Gety() - RAxis0;
  }

  for (size_t i = 0; i < 3; ++i)
  {
    const Edge &edge = *edgeList[i];

    const Vector<DoubleType> &p0 = ConvertVector<DoubleType>(edge.GetHead()->Position());
    const Vector<DoubleType> &p1 = ConvertVector<DoubleType>(edge.GetTail()->Position());

    ///// This is the midpoint along the edge
    Vector<DoubleType> vm = p0;
    vm += p1;
    vm *= 0.5;

    if (RAxisVariable == "x")
    {
      r0 = vm.Getx() - RAxis0;
    }
    else if (RAxisVariable == "y")
    {
      r0 = vm.Gety() - RAxis0;
    }

    ///// subtract out the center to get the length of the edge
    vm -= vc;

    //// equivalent to pi * (r1^2 - r0^2) / cos_theta
    //// r*cos_theta = s  where s is the coordinate along the line
    const DoubleType val = M_PI * (r1 + r0) * vm.magnitude();

    ec[i] = abs(val);
  }

  return Vector<DoubleType>(ec[0], ec[1], ec[2]);
}

template <typename DoubleType>
void TriangleCylindricalEdgeCouple<DoubleType>::Serialize(std::ostream &of) const
{
  of << "COMMAND element_cylindrical_edge_couple -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\"";
}

template class TriangleCylindricalEdgeCouple<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class TriangleCylindricalEdgeCouple<float128>;
#endif

TriangleEdgeModelPtr CreateTriangleCylindricalEdgeCouple(RegionPtr rp)
{
  const bool use_extended = rp->UseExtendedPrecisionModels();
  return create_triangle_edge_model<TriangleCylindricalEdgeCouple<double>, TriangleCylindricalEdgeCouple<extended_type>>(use_extended, rp);
}


