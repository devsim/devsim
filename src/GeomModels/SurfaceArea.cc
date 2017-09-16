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

    nsurf_x = NodeSolution<DoubleType>::CreateNodeSolution("NSurfaceNormal_x", rp, this->GetSelfPtr());
    nsurf_y = NodeSolution<DoubleType>::CreateNodeSolution("NSurfaceNormal_y", rp, this->GetSelfPtr());
  }
  else if (dimension == 3)
  {
    nsurf_x = NodeSolution<DoubleType>::CreateNodeSolution("NSurfaceNormal_x", rp, this->GetSelfPtr());
    nsurf_y = NodeSolution<DoubleType>::CreateNodeSolution("NSurfaceNormal_y", rp, this->GetSelfPtr());
    nsurf_z = NodeSolution<DoubleType>::CreateNodeSolution("NSurfaceNormal_z", rp, this->GetSelfPtr());
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


#if 0
  const Device::CoordinateIndexToInterface_t &cti = device.GetCoordinateIndexToInterface();
  const Device::CoordinateIndexToContact_t   &ctc = device.GetCoordinateIndexToContact();
  const ConstEdgeList &el = region.GetEdgeList();
#endif


  const ConstNodeList &nl = region.GetNodeList();
  std::vector<DoubleType> nv(nl.size());

  std::vector<size_t> edge_visited(region.GetNumberEdges());


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

    const DoubleType length = 0.5*edgeLengths[ei];
    nv[ni0] += length;
    nv[ni1] += length;


    Vector<DoubleType> vvec(unity[ei]*length, -unitx[ei]*length, 0.0);

    Vector<DoubleType> un0(nvx[ni0], nvy[ni0]);
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

    Vector<DoubleType> un1(nvx[ni1], nvy[ni1]);
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
    const DoubleType area = nv[i];
    if (area > 0.0)
    {
      const DoubleType wt = magnitude(Vector<DoubleType>(nvx[i], nvy[i], 0.0));
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
template <typename DoubleType>
void ProcessAreaAndNormal(size_t ni0, std::vector<DoubleType> &nv, std::vector<DoubleType> &nvx, std::vector<DoubleType> &nvy, std::vector<DoubleType> &nvz, const Vector<DoubleType> &vnormal, DoubleType volume)
{
  nv[ni0] += volume;

  Vector<DoubleType> uvec(nvx[ni0], nvy[ni0], nvz[ni0]);

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

template <typename DoubleType>
void SurfaceArea<DoubleType>::calcSurfaceArea3d() const
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
  std::vector<DoubleType> nvx(nl.size());
  std::vector<DoubleType> nvy(nl.size());
  std::vector<DoubleType> nvz(nl.size());

  const std::vector<Vector<DoubleType>> &triangleCenters = region.GetTriangleCenters<DoubleType>();

  std::vector<DoubleType> nv(nl.size());

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

    const Vector<DoubleType> &triangleCenter = triangleCenters[ti];

    const Vector<DoubleType> &np0 = nodeList[0]->Position();
    const Vector<DoubleType> &np1 = nodeList[1]->Position();
    const Vector<DoubleType> &np2 = nodeList[2]->Position();

    //// vector from triangle center to edge node
    //// vector between edge nodes
    //// area is 0.5 * base * height
    //// area split between both nodes

    const Vector<DoubleType> &v01   = np0 - np1;
    const Vector<DoubleType> &vc01  = np0 - triangleCenter;
    const Vector<DoubleType> &vec01 = 0.25 * cross_prod(v01, vc01);
    const DoubleType  vol01 = magnitude(vec01);

    ProcessAreaAndNormal(ni0, nv, nvx, nvy, nvz, vec01, vol01);
    ProcessAreaAndNormal(ni1, nv, nvx, nvy, nvz, vec01, vol01);

    const Vector<DoubleType> &v02   = np0 - np2;
    const Vector<DoubleType> &vc02  = np0 - triangleCenter;
    const Vector<DoubleType> &vec02 = 0.25 * cross_prod(v02, vc02);
    const DoubleType  vol02 = magnitude(vec02);

    ProcessAreaAndNormal(ni0, nv, nvx, nvy, nvz, vec02, vol02);
    ProcessAreaAndNormal(ni2, nv, nvx, nvy, nvz, vec02, vol02);

    const Vector<DoubleType> &v12   = np1 - np2;
    const Vector<DoubleType> &vc12  = np1 - triangleCenter;
    const Vector<DoubleType> &vec12 = 0.25 * cross_prod(v12, vc12);
    const DoubleType  vol12 = magnitude(vec12);

    ProcessAreaAndNormal(ni1, nv, nvx, nvy, nvz, vec12, vol12);
    ProcessAreaAndNormal(ni2, nv, nvx, nvy, nvz, vec12, vol12);
#if 0
    }
#endif
  }

  for (size_t i = 0; i < nv.size(); ++i)
  {
    const DoubleType area = nv[i];
    if (area > 0.0)
    {
      const DoubleType wt = magnitude(Vector<DoubleType>(nvx[i], nvy[i], nvz[i]));
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

