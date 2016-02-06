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

#include "CylindricalSurfaceArea.hh"
#include "Region.hh"
#include "Device.hh"
#include "dsAssert.hh"
#include "GeometryStream.hh"
#include "Contact.hh"
#include "Interface.hh"
#include "GlobalData.hh"
#include "Node.hh"
#include "Edge.hh"
#include "ObjectHolder.hh"
#include <sstream>


CylindricalSurfaceArea::CylindricalSurfaceArea(RegionPtr rp) :
NodeModel("CylindricalSurfaceArea", rp, NodeModel::SCALAR)
{
  //// This needs to be enforced in the Tcl API
  //// need to validate this is an actual interface or contact name in the Tcl API

  const Region &region = this->GetRegion();
  const size_t dimension = region.GetDimension();
  dsAssert(dimension == 2, "CylindricalSurfaceArea 2d Only");

  RegisterCallback("@@@InterfaceChange");
  RegisterCallback("@@@ContactChange");
  RegisterCallback("raxis_zero");
  RegisterCallback("raxis_variable");
}


void CylindricalSurfaceArea::calcNodeScalarValues() const
{
  const size_t dimension=GetRegion().GetDimension();

  if (dimension == 1)
  {
    dsAssert(false, "CylindricalSurfaceArea not supported in 1d");
  }
  else if (dimension == 2)
  {
    calcCylindricalSurfaceArea2d();
  }
  else if (dimension == 3)
  {
    dsAssert(false, "CylindricalSurfaceArea not supported in 3d");
  }
  else
  {
    dsAssert(false, "UNEXPECTED");
  }
}

namespace {
//// refactor to just rely on edge length formula
void CalcAreasOnEdges(const ConstEdgeList &edges, std::vector<double> &out, const std::string &RAxisVariable, double RAxis0)
{

  std::set<size_t> visited;

  double r0 = 0.0;
  double r1 = 0.0;
  double rm = 0.0;

  for (ConstEdgeList::const_iterator it = edges.begin(); it != edges.end(); ++it)
  {
    const Edge &edge = *(*it);

    const size_t edge_index = edge.GetIndex();

    if (!visited.count(edge_index))
    {
      visited.insert(edge_index);
    }
    else
    {
      continue;
    }

    const Node &node0 = *(edge.GetHead());
    const Node &node1 = *(edge.GetTail());

    const Vector &p0 = node0.Position();
    const Vector &p1 = node1.Position();

    /// This is the midpoint between the edges
    Vector vm = p1;
    vm       -= p0;
    vm       *= 0.5;

    const double vmag = vm.magnitude();

    if (RAxisVariable == "x")
    {
      r0 = p0.Getx() - RAxis0;
      r1 = p1.Getx() - RAxis0;
    }
    else if (RAxisVariable == "y")
    {
      r0 = p0.Gety() - RAxis0;
      r1 = p1.Gety() - RAxis0;
    }

    //// This must be in between the two points along the edge
    rm = 0.5 * (r0 + r1);

    const size_t ni0 = node0.GetIndex();
    out[ni0] += M_PI * fabs(rm + r0) * vmag;
    const size_t ni1 = node1.GetIndex();
    out[ni1] += M_PI * fabs(r1 + rm) * vmag;

  }
}
}

void CylindricalSurfaceArea::calcCylindricalSurfaceArea2d() const
{
  const Region &region = GetRegion();
  const Device &device = *(GetRegion().GetDevice());

  std::vector<double> nv(region.GetNumberNodes());

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

  const Device::ContactList_t   &contact_list   = device.GetContactList();
  const Device::InterfaceList_t &interface_list = device.GetInterfaceList();

  ConstEdgeList edge_list;

  for (Device::ContactList_t::const_iterator cit = contact_list.begin(); cit != contact_list.end(); ++cit)
  {
    const ConstContactPtr &cp = cit->second;
    if (cp && (cp->GetRegion() == &region))
    {
      const ConstEdgeList_t &temp_edge_list = cp->GetEdges();
      for (ConstEdgeList_t::const_iterator tit = temp_edge_list.begin(); tit != temp_edge_list.end(); ++tit)
      {
        edge_list.push_back(*tit);
      }
    }
  }
  for (Device::InterfaceList_t::const_iterator iit = interface_list.begin(); iit != interface_list.end(); ++iit)
  {
    const ConstInterfacePtr &ip = iit->second;
    if (ip)
    {
      if (ip->GetRegion0() == &region)
      {
        const ConstEdgeList_t &temp_edge_list = ip->GetEdges0();
        for (ConstEdgeList_t::const_iterator tit = temp_edge_list.begin(); tit != temp_edge_list.end(); ++tit)
        {
          edge_list.push_back(*tit);
        }
      }
      else if (ip->GetRegion1() == &region)
      {
        const ConstEdgeList_t &temp_edge_list = ip->GetEdges1();
        for (ConstEdgeList_t::const_iterator tit = temp_edge_list.begin(); tit != temp_edge_list.end(); ++tit)
        {
          edge_list.push_back(*tit);
        }
      }
    }
  }

  CalcAreasOnEdges(edge_list, nv, RAxisVariable, RAxis0);

  SetValues(nv);
}


void CylindricalSurfaceArea::Serialize(std::ostream &of) const
{
  of << "COMMAND cylindrical_surface_area -device \"" << GetRegion().GetDeviceName() << "\""
     << " -region \"" << GetRegionName() << "\"";
}

void CylindricalSurfaceArea::setInitialValues()
{
    DefaultInitializeValues();
}

