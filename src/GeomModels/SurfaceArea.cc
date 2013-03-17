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

#include "SurfaceArea.hh"
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

SurfaceArea::SurfaceArea(RegionPtr rp)
    : NodeModel("SurfaceArea", rp, NodeModel::SCALAR)
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

    nsurf_x = NodeSolution::CreateNodeSolution("NSurfaceNormal_x", rp, this->GetSelfPtr());
    nsurf_y = NodeSolution::CreateNodeSolution("NSurfaceNormal_y", rp, this->GetSelfPtr());
  }
  else if (dimension == 3)
  {
    nsurf_x = NodeSolution::CreateNodeSolution("NSurfaceNormal_x", rp, this->GetSelfPtr());
    nsurf_y = NodeSolution::CreateNodeSolution("NSurfaceNormal_y", rp, this->GetSelfPtr());
    nsurf_z = NodeSolution::CreateNodeSolution("NSurfaceNormal_z", rp, this->GetSelfPtr());
  }
  RegisterCallback("@@@InterfaceChange");
  RegisterCallback("@@@ContactChange");
}

void SurfaceArea::calcSurfaceArea1d() const
{
    ConstEdgeModelPtr elen = GetRegion().GetEdgeModel("EdgeLength");
    dsAssert(elen, "UNEXPECTED");

    const Device &device = *GetRegion().GetDevice();

    const Device::CoordinateIndexToInterface_t &cti = device.GetCoordinateIndexToInterface();
    const Device::CoordinateIndexToContact_t   &ctc = device.GetCoordinateIndexToContact();


    const ConstNodeList &nl = GetRegion().GetNodeList();

    std::vector<double> nv(nl.size());

    for (ConstNodeList::const_iterator it = nl.begin(); it != nl.end(); ++it)
    {
        const Node &node = *(*it);
        const size_t ci  = node.GetCoordinate().GetIndex();
        const size_t ni  = node.GetIndex();

        if (cti.count(ci) || ctc.count(ci))
        {
            const double length = 1.0;
            nv[ni] += length;
        }
    }
    SetValues(nv);
}

#if 0
namespace {
template <typename T, typename U>
bool SameInterface2D(T &cti, size_t i0, size_t i1)
{
  bool ret = false;
  if (cti.count(i0) && cti.count(i1))
  {
    const typename T::const_iterator it0 = cti.find(i0);
    const typename T::const_iterator it1 = cti.find(i1);
    const std::vector<U> &n0 = it0->second;
    const std::vector<U> &n1 = it1->second;
    std::vector<U> nout;

    set_intersection(n0.begin(), n0.end(),
      n1.begin(), n1.end(),
      std::insert_iterator<std::vector<U> >(nout, nout.begin())
    );

    ret = !nout.empty();
  }
  return ret;
}

template <typename T, typename U>
bool SameInterface3D(T &cti, size_t i0, size_t i1, size_t i2)
{
  bool ret = false;
  if (cti.count(i0) && cti.count(i1) && cti.count(i2))
  {
    const typename T::const_iterator it0 = cti.find(i0);
    const typename T::const_iterator it1 = cti.find(i1);
    const typename T::const_iterator it2 = cti.find(i2);
    const std::vector<U> &n0 = it0->second;
    const std::vector<U> &n1 = it1->second;
    const std::vector<U> &n2 = it2->second;

    std::vector<U> nout0;
    std::vector<U> nout1;

    set_intersection(n0.begin(), n0.end(),
      n1.begin(), n1.end(),
      std::insert_iterator<std::vector<U> >(nout0, nout0.begin())
    );

    if (!nout0.empty())
    {
      set_intersection(nout0.begin(), nout0.end(),
        n2.begin(), n2.end(),
        std::insert_iterator<std::vector<U> >(nout1, nout1.begin())
      );

      ret = !nout1.empty();
    }

  }
  return ret;
}

}
#endif

void SurfaceArea::calcSurfaceArea2d() const
{
  const Region &region = GetRegion();
  const Device &device = *(region.GetDevice());

  ConstEdgeModelPtr elen = region.GetEdgeModel("EdgeLength");
  dsAssert(elen, "UNEXPECTED");

  ConstEdgeModelPtr ux = region.GetEdgeModel("unitx");
  dsAssert(ux, "UNEXPECTED");
  ConstEdgeModelPtr uy = region.GetEdgeModel("unity");
  dsAssert(uy, "UNEXPECTED");


#if 0
  const Device::CoordinateIndexToInterface_t &cti = device.GetCoordinateIndexToInterface();
  const Device::CoordinateIndexToContact_t   &ctc = device.GetCoordinateIndexToContact();
  const ConstEdgeList &el = region.GetEdgeList();
#endif


  const ConstNodeList &nl = region.GetNodeList();
  std::vector<double> nv(nl.size());

  std::vector<size_t> edge_visited(region.GetNumberEdges());


  dsAssert(!nsurf_x.expired(), "UNEXPECTED");
  dsAssert(!nsurf_y.expired(), "UNEXPECTED");

  //// This is the net surface area for the vector
  //// This is the xnormal and ynormal
  std::vector<double> nvx(nl.size());
  std::vector<double> nvy(nl.size());

  const EdgeScalarList &edgeLengths = elen->GetScalarValues();

  const EdgeScalarList &unitx = ux->GetScalarValues();
  const EdgeScalarList &unity = uy->GetScalarValues();

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

  for (ConstEdgeList::const_iterator it = edge_list.begin(); it != edge_list.end(); ++it)
  {
    const Edge &edge = *(*it);

    //// Here we ignore edges we already visited
    const size_t ei = edge.GetIndex();
    if (edge_visited[ei] == 0)
    {
      edge_visited[ei] = 1;
    }
    else
    {
      continue;
    }

    const Node &node0 = *edge.GetHead();
#if 0
    const size_t ci0  = node0.GetCoordinate().GetIndex();
#endif

    const Node &node1 = *edge.GetTail();
#if 0
    const size_t ci1  = node1.GetCoordinate().GetIndex();
#endif

    //// TODO: there is nothing to stop both an interface and a contact contributing to the surface area of the same node
    //// TODO: print warning about coincident interface, which is possible with this new approach
#if 0
    if (SameInterface2D<const Device::CoordinateIndexToInterface_t, InterfacePtr>(cti, ci0, ci1) || 
        SameInterface2D<const Device::CoordinateIndexToContact_t, ContactPtr>(ctc, ci0, ci1)
       )
    {
#endif
    const size_t ni0  = node0.GetIndex();
    const size_t ni1  = node1.GetIndex();

    const double length = 0.5*edgeLengths[ei];
    nv[ni0] += length;
    nv[ni1] += length;


    Vector vvec(unity[ei]*length, -unitx[ei]*length, 0.0);

    Vector un0(nvx[ni0], nvy[ni0]);
    if (dot_prod(vvec, un0) < 0.0)
    {
      nvx[ni0] -= vvec.Getx();
      nvy[ni0] -= vvec.Gety();
    }
    else
    {
      nvx[ni0] += vvec.Getx();
      nvy[ni0] += vvec.Gety();
    }

    Vector un1(nvx[ni1], nvy[ni1]);
    if (dot_prod(vvec, un1) < 0.0)
    {
      nvx[ni1] -= vvec.Getx();
      nvy[ni1] -= vvec.Gety();
    }
    else
    {
      nvx[ni1] += vvec.Getx();
      nvy[ni1] += vvec.Gety();
    }
#if 0
    }
#endif
  }
  for (size_t i = 0; i < nv.size(); ++i)
  {
    const double area = nv[i];
    if (area > 0.0)
    {
      const double wt = magnitude(Vector(nvx[i], nvy[i], 0.0));
      if (wt > 0.0)
      {
        nvx[i] /= wt;
        nvy[i] /= wt;
      }
    }
  }

  SetValues(nv);
  nsurf_x.lock()->SetValues(nvx);
  nsurf_y.lock()->SetValues(nvy);

}

namespace {
void ProcessAreaAndNormal(size_t ni0, std::vector<double> &nv, std::vector<double> &nvx, std::vector<double> &nvy, std::vector<double> &nvz, const Vector &vnormal, double volume)
{
  nv[ni0] += volume;

  Vector uvec(nvx[ni0], nvy[ni0], nvz[ni0]);

  if (dot_prod(uvec, vnormal) < 0.0)
  {
    nvx[ni0] -= vnormal.Getx();
    nvy[ni0] -= vnormal.Gety();
    nvz[ni0] -= vnormal.Getz();
  }
  else
  {
    nvx[ni0] += vnormal.Getx();
    nvy[ni0] += vnormal.Gety();
    nvz[ni0] += vnormal.Getz();
  }
}

}

void SurfaceArea::calcSurfaceArea3d() const
{
  const Region &region = GetRegion();
  const Device &device = *(region.GetDevice());

#if 0
  const Device::CoordinateIndexToInterface_t &cti = device.GetCoordinateIndexToInterface();
  const Device::CoordinateIndexToContact_t   &ctc = device.GetCoordinateIndexToContact();
  const ConstTriangleList &tl = GetRegion().GetTriangleList();
#endif

  const ConstNodeList     &nl = region.GetNodeList();

  dsAssert(!nsurf_x.expired(), "UNEXPECTED");
  dsAssert(!nsurf_y.expired(), "UNEXPECTED");
  dsAssert(!nsurf_z.expired(), "UNEXPECTED");

  //// This is the net surface area for the vector
  //// This is the xnormal and ynormal
  std::vector<double> nvx(nl.size());
  std::vector<double> nvy(nl.size());
  std::vector<double> nvz(nl.size());

  const std::vector<Vector> &triangleCenters = region.GetTriangleCenters();

  std::vector<double> nv(nl.size());

  std::vector<size_t> triangle_visited(region.GetNumberTriangles());

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

  for (ConstTriangleList::const_iterator it = triangle_list.begin(); it != triangle_list.end(); ++it)
  {
    const Triangle &triangle = *(*it);

    //// Here we ignore triangles we already visited
    const size_t ti = triangle.GetIndex();
    if (triangle_visited[ti] == 0)
    {
      triangle_visited[ti] = 1;
    }
    else
    {
      continue;
    }

    const ConstNodeList &nodeList = triangle.GetNodeList();

#if 0
    const size_t ci0  = nodeList[0]->GetCoordinate().GetIndex();
    const size_t ci1  = nodeList[1]->GetCoordinate().GetIndex();
    const size_t ci2  = nodeList[2]->GetCoordinate().GetIndex();

    //// TODO: there is nothing to stop both an interface and a contact contributing to the surface area
    if (SameInterface3D<const Device::CoordinateIndexToInterface_t, InterfacePtr>(cti, ci0, ci1, ci2) || 
        SameInterface3D<const Device::CoordinateIndexToContact_t, ContactPtr>(ctc, ci0, ci1, ci2)
       )
    {
#endif
    const size_t &ni0 = nodeList[0]->GetIndex();
    const size_t &ni1 = nodeList[1]->GetIndex();
    const size_t &ni2 = nodeList[2]->GetIndex();

    const Vector &triangleCenter = triangleCenters[ti];

    const Vector &np0 = nodeList[0]->Position();
    const Vector &np1 = nodeList[1]->Position();
    const Vector &np2 = nodeList[2]->Position();

    //// vector from triangle center to edge node
    //// vector between edge nodes
    //// area is 0.5 * base * height
    //// area split between both nodes

    const Vector &v01   = np0 - np1;
    const Vector &vc01  = np0 - triangleCenter;
    const Vector &vec01 = 0.25 * cross_prod(v01, vc01);
    const double  vol01 = magnitude(vec01);

    ProcessAreaAndNormal(ni0, nv, nvx, nvy, nvz, vec01, vol01);
    ProcessAreaAndNormal(ni1, nv, nvx, nvy, nvz, vec01, vol01);

    const Vector &v02   = np0 - np2;
    const Vector &vc02  = np0 - triangleCenter;
    const Vector &vec02 = 0.25 * cross_prod(v02, vc02);
    const double  vol02 = magnitude(vec02);

    ProcessAreaAndNormal(ni0, nv, nvx, nvy, nvz, vec02, vol02);
    ProcessAreaAndNormal(ni2, nv, nvx, nvy, nvz, vec02, vol02);

    const Vector &v12   = np1 - np2;
    const Vector &vc12  = np1 - triangleCenter;
    const Vector &vec12 = 0.25 * cross_prod(v12, vc12);
    const double  vol12 = magnitude(vec12);

    ProcessAreaAndNormal(ni1, nv, nvx, nvy, nvz, vec12, vol12);
    ProcessAreaAndNormal(ni2, nv, nvx, nvy, nvz, vec12, vol12);
#if 0
    }
#endif
  }

  for (size_t i = 0; i < nv.size(); ++i)
  {
    const double area = nv[i];
    if (area > 0.0)
    {
      const double wt = magnitude(Vector(nvx[i], nvy[i], nvz[i]));
      if (wt > 0.0)
      {
        nvx[i] /= wt;
        nvy[i] /= wt;
        nvz[i] /= wt;
      }
    }
  }
  SetValues(nv);
  nsurf_x.lock()->SetValues(nvx);
  nsurf_y.lock()->SetValues(nvy);
  nsurf_z.lock()->SetValues(nvz);
}

void SurfaceArea::calcNodeScalarValues() const
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

void SurfaceArea::setInitialValues()
{
    DefaultInitializeValues();
}

void SurfaceArea::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

