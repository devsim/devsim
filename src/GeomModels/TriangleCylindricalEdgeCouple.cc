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

TriangleCylindricalEdgeCouple::TriangleCylindricalEdgeCouple(RegionPtr rp) :
TriangleEdgeModel("ElementCylindricalEdgeCouple", rp, TriangleEdgeModel::SCALAR)
{
  RegisterCallback("raxis_zero");
  RegisterCallback("raxis_variable");
}


void TriangleCylindricalEdgeCouple::calcTriangleEdgeScalarValues() const
{
  const Region &region = GetRegion();

  const size_t dimension=region.GetDimension();

  dsAssert(dimension == 2, "UNEXPECTED");

  double RAxis0 = 0.0;
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
      GeometryStream::WriteOut(OutputStream::FATAL, GetRegion(), os.str());
    }
  }

  const ConstTriangleList &el = region.GetTriangleList();
  std::vector<double> ev(3*el.size());

  for (size_t i = 0; i < el.size(); ++i)
  {
    const Vector &v = calcTriangleCylindricalEdgeCouple(el[i], RAxisVariable, RAxis0);
    const size_t indexi = 3*i;
    ev[indexi]     = v.Getx();
    ev[indexi + 1] = v.Gety();
    ev[indexi + 2] = v.Getz();
  }

  SetValues(ev);
}

Vector TriangleCylindricalEdgeCouple::calcTriangleCylindricalEdgeCouple(ConstTrianglePtr tp, const std::string &RAxisVariable, double RAxis0) const
{
  const Region &region = GetRegion();

  const Triangle &triangle = *tp;
  const std::vector<Vector> &centers = region.GetTriangleCenters();

  const Region::TriangleToConstEdgeList_t &ttelist = region.GetTriangleToEdgeList();
  size_t tindex = triangle.GetIndex();
  const ConstEdgeList &edgeList = ttelist[tindex];

  const Vector &vc = centers[triangle.GetIndex()];

  double ec[3];

  double r0 = 0.0;
  double r1 = 0.0;


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

    const Vector &p0 = edge.GetHead()->Position();
    const Vector &p1 = edge.GetTail()->Position();

    ///// This is the midpoint along the edge
    Vector vm = p0;
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
    const double val = M_PI * (r1 + r0) * vm.magnitude();

    ec[i] = fabs(val);
  }

  return Vector(ec[0], ec[1], ec[2]);
}

void TriangleCylindricalEdgeCouple::Serialize(std::ostream &of) const
{
  of << "COMMAND element_cylindrical_edge_couple -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\"";
}

