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

#include "SurfaceArea.hh"
#include "SurfaceAreaUtil.hh"
#include "Node.hh"
#include "Edge.hh"
#include "Triangle.hh"
#include "Vector.hh"
#include "EdgeModel.hh"
#include "Region.hh"
#include "Device.hh"
#include "dsAssert.hh"
#include "NodeSolution.hh"
#include "Contact.hh"
#include "Interface.hh"

#include <iterator>
#include <utility>

template <typename DoubleType>
SurfaceArea<DoubleType>::SurfaceArea(RegionPtr rp)
    : NodeModel("SurfaceArea", rp, NodeModel::DisplayType::SCALAR)
{
  const size_t dimension = GetRegion().GetDimension();
  if (dimension == 1)
  {
    RegisterCallback("EdgeLength");
  }
  if (dimension == 2)
  {
    RegisterCallback("EdgeLength");
    RegisterCallback("unitx");
    RegisterCallback("unity");

    nsurf_x = CreateNodeSolution("NSurfaceNormal_x", rp, this->GetSelfPtr());
    nsurf_y = CreateNodeSolution("NSurfaceNormal_y", rp, this->GetSelfPtr());
  }
  else if (dimension == 3)
  {
    nsurf_x = CreateNodeSolution("NSurfaceNormal_x", rp, this->GetSelfPtr());
    nsurf_y = CreateNodeSolution("NSurfaceNormal_y", rp, this->GetSelfPtr());
    nsurf_z = CreateNodeSolution("NSurfaceNormal_z", rp, this->GetSelfPtr());
  }
  RegisterCallback("@@@InterfaceChange");
  RegisterCallback("@@@ContactChange");
}

template <typename DoubleType>
void SurfaceArea<DoubleType>::calcSurfaceArea1d() const
{
    ConstEdgeModelPtr elen = GetRegion().GetEdgeModel("EdgeLength");
    dsAssert(elen.get(), "UNEXPECTED");

    const Device &device = *GetRegion().GetDevice();

    const Device::CoordinateIndexToInterface_t &cti = device.GetCoordinateIndexToInterface();
    const Device::CoordinateIndexToContact_t   &ctc = device.GetCoordinateIndexToContact();


    const ConstNodeList &nl = GetRegion().GetNodeList();

    std::vector<DoubleType> nv(nl.size());

    for (ConstNodeList::const_iterator it = nl.begin(); it != nl.end(); ++it)
    {
        const Node &node = *(*it);
        const size_t ci  = node.GetCoordinate().GetIndex();
        const size_t ni  = node.GetIndex();

        if (cti.count(ci) || ctc.count(ci))
        {
            const DoubleType length = 1.0;
            nv[ni] += length;
        }
    }
    SetValues(nv);
}

template <typename DoubleType>
void SurfaceArea<DoubleType>::calcSurfaceArea2d() const
{
  const Region &region = GetRegion();
  const Device &device = *(region.GetDevice());

  ConstEdgeModelPtr elen = region.GetEdgeModel("EdgeLength");
  dsAssert(elen.get(), "UNEXPECTED");

  ConstEdgeModelPtr ux = region.GetEdgeModel("unitx");
  dsAssert(ux.get(), "UNEXPECTED");
  ConstEdgeModelPtr uy = region.GetEdgeModel("unity");
  dsAssert(uy.get(), "UNEXPECTED");

  const ConstNodeList &nl = region.GetNodeList();
  std::vector<DoubleType> nv(nl.size());

  dsAssert(!nsurf_x.expired(), "UNEXPECTED");
  dsAssert(!nsurf_y.expired(), "UNEXPECTED");

  //// This is the net surface area for the vector
  //// This is the xnormal and ynormal
  std::vector<DoubleType> nvx(nl.size());
  std::vector<DoubleType> nvy(nl.size());

  const EdgeScalarList<DoubleType> &edgeLengths = elen->GetScalarValues<DoubleType>();

  const EdgeScalarList<DoubleType> &unitx = ux->GetScalarValues<DoubleType>();
  const EdgeScalarList<DoubleType> &unity = uy->GetScalarValues<DoubleType>();

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

  SurfaceAreaUtil::processEdgeList(edge_list, unitx, unity, edgeLengths, nv, nvx, nvy);

  SetValues(nv);
  nsurf_x.lock()->SetValues(nvx);
  nsurf_y.lock()->SetValues(nvy);
}

template <typename DoubleType>
void SurfaceArea<DoubleType>::calcSurfaceArea3d() const
{
  const Region &region = GetRegion();
  const Device &device = *(region.GetDevice());

  const ConstNodeList     &nl = region.GetNodeList();

  dsAssert(!nsurf_x.expired(), "UNEXPECTED");
  dsAssert(!nsurf_y.expired(), "UNEXPECTED");
  dsAssert(!nsurf_z.expired(), "UNEXPECTED");

  //// This is the net surface area for the vector
  //// This is the xnormal and ynormal
  std::vector<DoubleType> nvx(nl.size());
  std::vector<DoubleType> nvy(nl.size());
  std::vector<DoubleType> nvz(nl.size());

  const std::vector<Vector<DoubleType>> &triangleCenters = region.GetTriangleCenters<DoubleType>();

  std::vector<DoubleType> nv(nl.size());


  Device::ContactList_t   contact_list   = device.GetContactList();
  Device::InterfaceList_t interface_list = device.GetInterfaceList();

  ConstTriangleList triangle_list;

  for (Device::ContactList_t::const_iterator cit = contact_list.begin(); cit != contact_list.end(); ++cit)
  {
    const ConstContactPtr &cp = cit->second;
    if (cp && (cp->GetRegion() == &region))
    {
      const ConstTriangleList_t &temp_triangle_list = cp->GetTriangles();
      for (ConstTriangleList_t::const_iterator tit = temp_triangle_list.begin(); tit != temp_triangle_list.end(); ++tit)
      {
        triangle_list.push_back(*tit);
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
        const ConstTriangleList_t &temp_triangle_list = ip->GetTriangles0();
        for (ConstTriangleList_t::const_iterator tit = temp_triangle_list.begin(); tit != temp_triangle_list.end(); ++tit)
        {
          triangle_list.push_back(*tit);
        }
      }
      else if (ip->GetRegion1() == &region)
      {
        const ConstTriangleList_t &temp_triangle_list = ip->GetTriangles1();
        for (ConstTriangleList_t::const_iterator tit = temp_triangle_list.begin(); tit != temp_triangle_list.end(); ++tit)
        {
          triangle_list.push_back(*tit);
        }
      }
    }
  }

  SurfaceAreaUtil::processTriangleList(triangle_list, triangleCenters, nv, nvx, nvy, nvz);

  SetValues(nv);
  nsurf_x.lock()->SetValues(nvx);
  nsurf_y.lock()->SetValues(nvy);
  nsurf_z.lock()->SetValues(nvz);
}

template <typename DoubleType>
void SurfaceArea<DoubleType>::calcNodeScalarValues() const
{
  const size_t dimension = GetRegion().GetDimension();

  if (dimension == 1)
  {
    calcSurfaceArea1d();
  }
  else if (dimension == 2)
  {
    calcSurfaceArea2d();
  }
  else if (dimension == 3)
  {
    calcSurfaceArea3d();
  }
}

template <typename DoubleType>
void SurfaceArea<DoubleType>::setInitialValues()
{
  DefaultInitializeValues();
}

template <typename DoubleType>
void SurfaceArea<DoubleType>::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

template class SurfaceArea<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class SurfaceArea<float128>;
#endif

