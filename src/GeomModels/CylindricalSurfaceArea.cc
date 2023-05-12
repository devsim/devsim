/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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
#include <cmath>
using std::abs;


template <typename DoubleType>
CylindricalSurfaceArea<DoubleType>::CylindricalSurfaceArea(RegionPtr rp) :
NodeModel("CylindricalSurfaceArea", rp, NodeModel::DisplayType::SCALAR)
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


template <typename DoubleType>
void CylindricalSurfaceArea<DoubleType>::calcNodeScalarValues() const
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
template <typename DoubleType>
void CalcAreasOnEdges(const ConstEdgeList &edges, std::vector<DoubleType> &out, const std::string &RAxisVariable, DoubleType RAxis0)
{

  std::set<size_t> visited;

  DoubleType r0 = 0.0;
  DoubleType r1 = 0.0;
  DoubleType rm = 0.0;

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

    const Vector<DoubleType> &p0 = ConvertVector<DoubleType>(node0.Position());
    const Vector<DoubleType> &p1 = ConvertVector<DoubleType>(node1.Position());

    /// This is the midpoint between the edges
    Vector<DoubleType> vm = p1;
    vm       -= p0;
    vm       *= 0.5;

    const DoubleType vmag = vm.magnitude();

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
    out[ni0] += M_PI * abs(rm + r0) * vmag;
    const size_t ni1 = node1.GetIndex();
    out[ni1] += M_PI * abs(r1 + rm) * vmag;

  }
}
}

template <typename DoubleType>
void CylindricalSurfaceArea<DoubleType>::calcCylindricalSurfaceArea2d() const
{
  const Region &region = GetRegion();
  const Device &device = *(GetRegion().GetDevice());

  std::vector<DoubleType> nv(region.GetNumberNodes());

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


template <typename DoubleType>
void CylindricalSurfaceArea<DoubleType>::Serialize(std::ostream &of) const
{
  of << "COMMAND cylindrical_surface_area -device \"" << GetRegion().GetDeviceName() << "\""
     << " -region \"" << GetRegionName() << "\"";
}

template <typename DoubleType>
void CylindricalSurfaceArea<DoubleType>::setInitialValues()
{
    DefaultInitializeValues();
}

template class CylindricalSurfaceArea<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class CylindricalSurfaceArea<float128>;
#endif

NodeModelPtr CreateCylindricalSurfaceArea(RegionPtr rp)
{
  const bool use_extended = rp->UseExtendedPrecisionModels();
  return create_node_model<CylindricalSurfaceArea<double>, CylindricalSurfaceArea<extended_type>>(use_extended, rp);
}

