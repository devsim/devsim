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

#include "TriangleCylindricalNodeVolume.hh"
#include "dsAssert.hh"
#include "TriangleEdgeScalarData.hh"
#include "Region.hh"
#include "EdgeModel.hh"
#include "Edge.hh"
#include "TriangleEdgeSubModel.hh"
#include "Node.hh"
#include "Triangle.hh"
#include "Vector.hh"
#include "GlobalData.hh"
#include "GeometryStream.hh"
#include "ObjectHolder.hh"

#include <algorithm>
#include <vector>

//// TODO: it might be helpful to node this at node @en2 as well
TriangleCylindricalNodeVolume::TriangleCylindricalNodeVolume(RegionPtr rp)
    : TriangleEdgeModel("ElementCylindricalNodeVolume@en0", rp, TriangleEdgeModel::SCALAR)
{
  node1Volume_ = TriangleEdgeSubModel::CreateTriangleEdgeSubModel("ElementCylindricalNodeVolume@en1", rp, TriangleEdgeModel::SCALAR, this->GetSelfPtr());
  RegisterCallback("raxis_zero");
  RegisterCallback("raxis_variable");
}

void TriangleCylindricalNodeVolume::calcTriangleEdgeScalarValues() const
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
  std::vector<double> ev0(3*el.size());
  std::vector<double> ev1(3*el.size());
  std::vector<double> nv(region.GetNumberNodes());

  for (size_t i = 0; i < el.size(); ++i)
  {
    const std::vector<double> &v = calcTriangleCylindricalNodeVolume(el[i], RAxisVariable, RAxis0);
    const size_t indexi = 3*i;
    ev0[indexi]     = v[0];
    ev0[indexi + 1] = v[1];
    ev0[indexi + 2] = v[2];
    ev1[indexi]     = v[3];
    ev1[indexi + 1] = v[4];
    ev1[indexi + 2] = v[5];
  }

  SetValues(ev0);
  node1Volume_.lock()->SetValues(ev1);
}

//// TODO: detect when radii crosses z axis before swept
namespace {
// Calc volume of triangle with flat top or bottom (constant z) on at least one side
// assume r1 and r2 are at z1
double calcCylindricalTriangleVolumeFlat(double r0, double z0, double r1, double z1, double r2)
{
  double ret = 0.0;
  const double z = fabs(z1 - z0);
  const double r20_2 = pow((r2 - r0), 2.0);
  const double r10_2 = pow((r1 - r0), 2.0);

  ret = fabs(r20_2 - r10_2) * z / 3.0;

  const double r21 = fabs(r2 - r1);
  ret += r21 * r0 * z;
  ret *= M_PI; 

  return ret;
}

// Calc volume of arbitrary triangle swept around z axis
double calcCylindricalTriangleVolume(std::vector<std::pair<double, double> > &pairs)
{
  double ret = 0.0;
  /// sort by height
  std::sort(pairs.begin(), pairs.end());

  const double &z0 = pairs[0].first;
  const double &r0 = pairs[0].second;
  const double &z1 = pairs[1].first;
  const double &r1 = pairs[1].second;
  const double &z2 = pairs[2].first;
  const double &r2 = pairs[2].second;

  if (z0 == z1)
  {
    ret = calcCylindricalTriangleVolumeFlat(r2, z2, r0, z0, r1);
  }
  else if (z1 == z2)
  {
    ret = calcCylindricalTriangleVolumeFlat(r0, z0, r1, z1, r2);
  }
  else
  {
    //// this is the midpoint (radius) drawn directly between z0 and z2
    //// since we are sorted by increasing z, a good triangle should have a valid denominator
    const double r3 = (z1 - z0) * (r2 - r0) / (z2 - z0) + r0;

    ret  = calcCylindricalTriangleVolumeFlat(r0, z0, r1, z1, r3);
    ret += calcCylindricalTriangleVolumeFlat(r2, z2, r1, z1, r3);

  }

  return ret;
}
}

std::vector<double> TriangleCylindricalNodeVolume::calcTriangleCylindricalNodeVolume(ConstTrianglePtr tp, const std::string &RAxisVariable, double RAxis0) const
{
  const Region &region = GetRegion();

  const Triangle &triangle = *tp;
  const std::vector<Vector> &centers = region.GetTriangleCenters();

  const Region::TriangleToConstEdgeList_t &ttelist = region.GetTriangleToEdgeList();
  size_t tindex = triangle.GetIndex();
  const ConstEdgeList &edgeList = ttelist[tindex];

  const Vector &vc = centers[triangle.GetIndex()];

  //// These are the returned volumes
  std::vector<double> ret(6);

  double r0 = 0.0; // node 0
  double r1 = 0.0; // triangle center
  double r2 = 0.0; // edge midpoint
  double r3 = 0.0; // node 1

  double z0 = 0.0;
  double z1 = 0.0;
  double z2 = 0.0;
  double z3 = 0.0;



  if (RAxisVariable == "x")
  {
    r1 = vc.Getx() - RAxis0;
    z1 = vc.Gety();
  }
  else if (RAxisVariable == "y")
  {
    r1 = vc.Gety() - RAxis0;
    z1 = vc.Getx();
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
      r2 = vm.Getx() - RAxis0;
      z2 = vm.Gety();
      r0 = p0.Getx() - RAxis0;
      z0 = p0.Gety();
      r3 = p1.Getx() - RAxis0;
      z3 = p1.Gety();

    }
    else if (RAxisVariable == "y")
    {
      r2 = vm.Gety() - RAxis0;
      z2 = vm.Getx();
      r0 = p0.Gety() - RAxis0;
      z0 = p0.Getx();
      r3 = p1.Gety() - RAxis0;
      z3 = p1.Getx();
    }

    std::vector<std::pair<double, double> > pairs(3);

    pairs[0] = std::make_pair(z0, r0);
    pairs[1] = std::make_pair(z1, r1);
    pairs[2] = std::make_pair(z2, r2);
    const double val0 = calcCylindricalTriangleVolume(pairs);
    ret[i]   = val0;

    pairs[0] = std::make_pair(z3, r3);
    pairs[1] = std::make_pair(z1, r1);
    pairs[2] = std::make_pair(z2, r2);
    const double val1 = calcCylindricalTriangleVolume(pairs);
    ret[i+3] = val1;
  }

  return ret;
}

void TriangleCylindricalNodeVolume::Serialize(std::ostream &of) const
{
  of << "COMMAND element_cylindrical_node_volume -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\"";
}

