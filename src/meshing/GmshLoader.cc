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

#include "GmshLoader.hh"
#include "OutputStream.hh"
#include "Region.hh"
#include "GlobalData.hh"
#include "Device.hh"
#include "Coordinate.hh"
#include "Contact.hh"
#include "Interface.hh"
#include "dsAssert.hh"
#include "Node.hh"
#include "Edge.hh"
#include "Triangle.hh"
#include "Tetrahedron.hh"
#include "ModelCreate.hh"
#include "MeshLoaderUtility.hh"

#include <sstream>

namespace dsMesh {

namespace {
void processNodes(const MeshNodeList_t &mnlist, const std::vector<Coordinate *> &clist, std::vector<Node *> &node_list)
{

  node_list.clear();
  node_list.resize(clist.size());

  MeshNodeList_t::const_iterator mnit = mnlist.begin();
  for (; mnit != mnlist.end(); ++mnit)
  {
    const size_t index = mnit->Index();

    dsAssert(index < clist.size(), "UNEXPECTED");

    dsAssert(clist[index] != 0, "UNEXPECTED");

    Node *tp = new Node(index, clist[index]);
    node_list[index] = tp;
  }
}

void processEdges(const MeshEdgeList_t &tl, const std::vector<Node *> &nlist, std::vector<const Edge *> &edge_list)
{

  MeshEdgeList_t::const_iterator tit = tl.begin();
  for (size_t ti = 0; tit != tl.end(); ++tit, ++ti)
  {
  
      const size_t index0 = tit->Index0();
      const size_t index1 = tit->Index1();

      dsAssert(index0 < nlist.size(), "UNEXPECTED");
      dsAssert(index1 < nlist.size(), "UNEXPECTED");

      dsAssert(nlist[index0] != 0, "UNEXPECTED");
      dsAssert(nlist[index1] != 0, "UNEXPECTED");

      Edge *tp = new Edge(ti, nlist[index0], nlist[index1]);
      edge_list.push_back(tp);
  }
}

void processTriangles(const MeshTriangleList_t &tl, const std::vector<Node *> &nlist, std::vector<const Triangle *> &triangle_list)
{

  MeshTriangleList_t::const_iterator tit = tl.begin();
  for (size_t ti = 0; tit != tl.end(); ++tit, ++ti)
  {
  
      const size_t index0 = tit->Index0();
      const size_t index1 = tit->Index1();
      const size_t index2 = tit->Index2();
      dsAssert(index0 < nlist.size(), "UNEXPECTED");
      dsAssert(index1 < nlist.size(), "UNEXPECTED");
      dsAssert(index2 < nlist.size(), "UNEXPECTED");
//      size_t nlistsize = nlist.size();

      dsAssert(nlist[index0] != 0, "UNEXPECTED");
      dsAssert(nlist[index1] != 0, "UNEXPECTED");
      dsAssert(nlist[index2] != 0, "UNEXPECTED");

      Triangle *tp = new Triangle(ti, nlist[index0], nlist[index1], nlist[index2]);
      triangle_list.push_back(tp);
  }
}

void processTetrahedra(const MeshTetrahedronList_t &tl, const std::vector<Node *> &nlist, std::vector<const Tetrahedron *> &tetrahedron_list)
{

  MeshTetrahedronList_t::const_iterator tit = tl.begin();
  for (size_t ti = 0; tit != tl.end(); ++tit, ++ti)
  {
  
      const size_t index0 = tit->Index0();
      const size_t index1 = tit->Index1();
      const size_t index2 = tit->Index2();
      const size_t index3 = tit->Index3();
      dsAssert(index0 < nlist.size(), "UNEXPECTED");
      dsAssert(index1 < nlist.size(), "UNEXPECTED");
      dsAssert(index2 < nlist.size(), "UNEXPECTED");
      dsAssert(index3 < nlist.size(), "UNEXPECTED");

      dsAssert(nlist[index0] != 0, "UNEXPECTED");
      dsAssert(nlist[index1] != 0, "UNEXPECTED");
      dsAssert(nlist[index2] != 0, "UNEXPECTED");
      dsAssert(nlist[index3] != 0, "UNEXPECTED");

//    std::cerr << "DEBUG "; 
//      std::cerr
//       << nlist[index0]->GetIndex() << "\t"
//       << nlist[index1]->GetIndex() << "\t"
//       << nlist[index2]->GetIndex() << "\t"
//       << nlist[index3]->GetIndex() << "\t"
//        << std::endl;
//      ;

      Tetrahedron *tp = new Tetrahedron(ti, nlist[index0], nlist[index1], nlist[index2], nlist[index3]);
      tetrahedron_list.push_back(tp);
  }
}
}

GmshLoader::GmshLoader(const std::string &n) : Mesh(n), dimension(0), maxCoordinateIndex(0)
{
    //// Arbitrary number
    meshCoordinateList.reserve(1000);
    //// handle dimensions 1,2,3
    physicalDimensionIndexNameMap.resize(4);
}
;
GmshLoader::~GmshLoader() {
#if 0
  DeletePointersFromVector<MeshCoordinateList_t>(meshCoordinateList);
  DeletePointersFromMap<MeshRegionList_t>(regionList);
      DeletePointersFromMap<MeshInterfaceList_t>(interfaceList);
      DeletePointersFromMap<MeshContactList_t>(contactList);
#endif
}


void GmshLoader::GetUniqueNodesFromPhysicalNames(const std::vector<std::string> &pnames, MeshNodeList_t &mnlist)
{
  mnlist.clear();
  for (std::vector<std::string>::const_iterator pit = pnames.begin(); pit != pnames.end(); ++pit)
  {
    const std::string &pname = *pit;

    const MeshNodeList_t &pnlist = gmshShapesMap[pname].Points;
    {
    std::ostringstream os; 
    os << "group " << pname << " has " << pnlist.size() << " nodes.\n";
    OutputStream::WriteOut(OutputStream::INFO, os.str());
    }


    for (MeshNodeList_t::const_iterator nit = pnlist.begin(); nit != pnlist.end(); ++nit)
    {
      mnlist.push_back(*nit);
    }
  }
  /// Remove overlapping nodes
  std::sort(mnlist.begin(), mnlist.end());
  MeshNodeList_t::iterator elnewend = std::unique(mnlist.begin(), mnlist.end());
  mnlist.erase(elnewend, mnlist.end());
}

void GmshLoader::GetUniqueTetrahedraFromPhysicalNames(const std::vector<std::string> &pnames, MeshTetrahedronList_t &mnlist)
{
  mnlist.clear();
  for (std::vector<std::string>::const_iterator pit = pnames.begin(); pit != pnames.end(); ++pit)
  {
    const std::string &pname = *pit;

    const MeshTetrahedronList_t &pnlist = gmshShapesMap[pname].Tetrahedra;

    for (MeshTetrahedronList_t::const_iterator nit = pnlist.begin(); nit != pnlist.end(); ++nit)
    {
      mnlist.push_back(*nit);
    }
  }
  /// Remove overlapping nodes
  std::sort(mnlist.begin(), mnlist.end());
  MeshTetrahedronList_t::iterator elnewend = std::unique(mnlist.begin(), mnlist.end());
  mnlist.erase(elnewend, mnlist.end());
}

void GmshLoader::GetUniqueTrianglesFromPhysicalNames(const std::vector<std::string> &pnames, MeshTriangleList_t &mnlist)
{
  mnlist.clear();
  for (std::vector<std::string>::const_iterator pit = pnames.begin(); pit != pnames.end(); ++pit)
  {
    const std::string &pname = *pit;

    const MeshTriangleList_t &pnlist = gmshShapesMap[pname].Triangles;

    for (MeshTriangleList_t::const_iterator nit = pnlist.begin(); nit != pnlist.end(); ++nit)
    {
      mnlist.push_back(*nit);
    }
  }
  /// Remove overlapping nodes
  std::sort(mnlist.begin(), mnlist.end());
  MeshTriangleList_t::iterator elnewend = std::unique(mnlist.begin(), mnlist.end());
  mnlist.erase(elnewend, mnlist.end());
}

void GmshLoader::GetUniqueEdgesFromPhysicalNames(const std::vector<std::string> &pnames, MeshEdgeList_t &mnlist)
{
  mnlist.clear();
  for (std::vector<std::string>::const_iterator pit = pnames.begin(); pit != pnames.end(); ++pit)
  {
    const std::string &pname = *pit;

    const MeshEdgeList_t &pnlist = gmshShapesMap[pname].Lines;

    for (MeshEdgeList_t::const_iterator nit = pnlist.begin(); nit != pnlist.end(); ++nit)
    {
      mnlist.push_back(*nit);
    }
  }
  /// Remove overlapping nodes
  std::sort(mnlist.begin(), mnlist.end());
  MeshEdgeList_t::iterator elnewend = std::unique(mnlist.begin(), mnlist.end());
  mnlist.erase(elnewend, mnlist.end());
}

bool GmshLoader::Instantiate_(const std::string &deviceName, std::string &errorString)
{
  bool ret = true;
  GlobalData &gdata = GlobalData::GetInstance();

  if (gdata.GetDevice(deviceName))
  {
      std::ostringstream os; 
      os << deviceName << " already exists\n";
      errorString += os.str();
      ret = false;
      return ret;
  }

  DevicePtr dp = new Device(deviceName, dimension);
  gdata.AddDevice(dp);

  Device::CoordinateList_t coordinate_list(maxCoordinateIndex+1);

  size_t ccount = 0;
  for (MeshCoordinateList_t::const_iterator it=meshCoordinateList.begin(); it != meshCoordinateList.end(); ++it)
  {
    const MeshCoordinate &mc = (it->second);
    CoordinatePtr new_coordinate =  new Coordinate(mc.GetX(), mc.GetY(), mc.GetZ()); 
    coordinate_list[it->first] = new_coordinate;
    dp->AddCoordinate(new_coordinate);
    ++ccount;
  }
  {
  std::ostringstream os; 
  os << "Device " << deviceName << " has " << ccount << " coordinates with max index " << maxCoordinateIndex << "\n";
  OutputStream::WriteOut(OutputStream::INFO, os.str());
  }

  std::map<std::string, std::vector<NodePtr> > RegionNameToNodeMap;

  //// For each name in the region map, we create a list of nodes which are indexes
  {
    MeshNodeList_t        mesh_nodes;
    MeshTetrahedronList_t mesh_tetrahedra;
    MeshTriangleList_t    mesh_triangles;
    MeshEdgeList_t        mesh_edges;

    ConstTetrahedronList              tetrahedronList;
    ConstTriangleList                 triangleList;
    ConstEdgeList                     edgeList;

    for (MapToRegionInfo_t::const_iterator rit = regionMap.begin(); rit != regionMap.end(); ++rit)
    {
      mesh_nodes.clear();
      mesh_tetrahedra.clear();
      mesh_triangles.clear();
      mesh_edges.clear();

      tetrahedronList.clear();
      triangleList.clear();
      edgeList.clear();

      const std::string &regionName = rit->first;
      const GmshRegionInfo &rinfo = rit->second;
      const std::vector<std::string> &pnames = rinfo.physical_names;



      Region *regionptr = dp->GetRegion(regionName);
      if (!regionptr)
      {
        regionptr = new Region(regionName, rinfo.material, dimension, dp);
        dp->AddRegion(regionptr);
      }

      Region &region = *regionptr;

      GetUniqueNodesFromPhysicalNames(pnames, mesh_nodes);

      {
      std::ostringstream os; 
      os << "Region " << regionName << " has " << mesh_nodes.size() << " nodes.\n";
      OutputStream::WriteOut(OutputStream::INFO, os.str());
      }


      std::vector<NodePtr> &nodeList = RegionNameToNodeMap[regionName];
      processNodes(mesh_nodes, coordinate_list, nodeList);

      size_t ncount = 0;
      for (std::vector<NodePtr>::const_iterator nit = nodeList.begin(); nit != nodeList.end(); ++nit)
      {
        NodePtr np = *nit;
        if (np)
        {
          region.AddNode(np);
          ++ncount;
        }
      }

      {
      std::ostringstream os; 
      os << "Region " << regionName << " has " << ncount << " nodes.\n";
      OutputStream::WriteOut(OutputStream::INFO, os.str());
      }

      if (dimension == 3)
      {
        GetUniqueTetrahedraFromPhysicalNames(pnames, mesh_tetrahedra);
        processTetrahedra(mesh_tetrahedra, nodeList, tetrahedronList);
        region.AddTetrahedronList(tetrahedronList);
      }

      if (dimension >= 2)
      {
        GetUniqueTrianglesFromPhysicalNames(pnames, mesh_triangles);
        processTriangles(mesh_triangles, nodeList, triangleList);
        region.AddTriangleList(triangleList);
      }
      GetUniqueEdgesFromPhysicalNames(pnames, mesh_edges);
      processEdges(mesh_edges, nodeList, edgeList);
      region.AddEdgeList(edgeList);
      region.FinalizeMesh();
      CreateDefaultModels(&region);
    }
  }

  //// Now process the contact
  {
    MeshNodeList_t mesh_nodes;
    ConstNodeList cnodes;
    for (MapToContactInfo_t::const_iterator cit = contactMap.begin(); cit != contactMap.end(); ++cit)
    {
      mesh_nodes.clear();
      const std::string &contactName = cit->first;
      const GmshContactInfo &cinfo   = cit->second;

      const std::string &regionName = cinfo.region;
      const std::string &materialName = cinfo.material;

      if (!RegionNameToNodeMap.count(regionName))
      {
        std::ostringstream os; 
        os << "Contact " << contactName << " references non-existent region name " << regionName << ".\n";
        OutputStream::WriteOut(OutputStream::ERROR, os.str());
        ret = false;
        continue;
      }

      const std::vector<NodePtr> &nodeList = RegionNameToNodeMap[regionName];

      const std::vector<std::string> &pnames = cinfo.physical_names;

      GetUniqueNodesFromPhysicalNames(pnames, mesh_nodes);

      Region *regionptr = dp->GetRegion(regionName);
      if (!regionptr)
      {
        std::ostringstream os; 
        os << "Contact " << contactName << " references non-existent region name " << regionName << ".\n";
        OutputStream::WriteOut(OutputStream::ERROR, os.str());
        ret = false;
        continue;
      }
  //    Region &region = *regionptr;

      Contact *contactptr = dp->GetContact(contactName);
      if (contactptr)
      {
        std::ostringstream os; 
        os << "Contact " << contactName << " on region " << regionName << " being instantiated multiple times.\n";
        OutputStream::WriteOut(OutputStream::ERROR, os.str());
        ret = false;
        continue;
      }

      cnodes.clear();
      for (MeshNodeList_t::const_iterator tnit = mesh_nodes.begin(); tnit != mesh_nodes.end(); ++tnit)
      {
        size_t cindex = tnit->Index();

        if (cindex > maxCoordinateIndex)
        {
          std::ostringstream os; 
          os << "Contact " << contactName << " reference coordinate " << cindex << " which does not exist on any region.\n";
          OutputStream::WriteOut(OutputStream::ERROR, os.str());
          ret = false;
          continue;
        }

        ConstNodePtr np = nodeList[cindex];
        if (np)
        {
          cnodes.push_back(np);
        }
        else
        {
          std::ostringstream os; 
          os << "Contact " << contactName << " reference coordinate " << cindex << " not on region " << regionName << ".\n";
          OutputStream::WriteOut(OutputStream::ERROR, os.str());
          ret = false;
          continue;
        }
      }

      if (cnodes.empty())
      {
        std::ostringstream os; 
        os << "Contact " << contactName << " does not reference any nodes on region " << regionName << " .\n";
        OutputStream::WriteOut(OutputStream::ERROR, os.str());
        ret = false;
        continue;
      }

      dp->AddContact(new Contact(contactName, regionptr, cnodes, materialName));
      std::ostringstream os; 
      os << "Contact " << contactName << " in region " << regionName << " with " << cnodes.size() << " nodes" << "\n";
      OutputStream::WriteOut(OutputStream::INFO, os.str());
    }
  }

  {
    ConstNodeList inodes0;
    ConstNodeList inodes1;
    MeshNodeList_t mesh_nodes;
    for (MapToInterfaceInfo_t::const_iterator iit = interfaceMap.begin(); iit != interfaceMap.end(); ++iit)
    {
      mesh_nodes.clear();
      const std::string &interfaceName = iit->first;
      const GmshInterfaceInfo &iinfo   = iit->second;

      const std::string &regionName0 = iinfo.region0;
      const std::string &regionName1 = iinfo.region1;

      if (!RegionNameToNodeMap.count(regionName0))
      {
        std::ostringstream os; 
        os << "Interface " << interfaceName << " references non-existent region name " << regionName0 << ".\n";
        OutputStream::WriteOut(OutputStream::ERROR, os.str());
        ret = false;
        continue;
      }
      if (!RegionNameToNodeMap.count(regionName1))
      {
        std::ostringstream os; 
        os << "Interface " << interfaceName << " references non-existent region name " << regionName1 << ".\n";
        OutputStream::WriteOut(OutputStream::ERROR, os.str());
        ret = false;
        continue;
      }

      const std::vector<NodePtr> &nodeList0 = RegionNameToNodeMap[regionName0];
      const std::vector<NodePtr> &nodeList1 = RegionNameToNodeMap[regionName1];

      const std::vector<std::string> &pnames = iinfo.physical_names;

      GetUniqueNodesFromPhysicalNames(pnames, mesh_nodes);

      Region *regionptr0 = dp->GetRegion(regionName0);
      Region *regionptr1 = dp->GetRegion(regionName1);
      if (!regionptr0)
      {
        std::ostringstream os; 
        os << "Interface " << interfaceName << " references non-existent region name " << regionName0 << ".\n";
        OutputStream::WriteOut(OutputStream::ERROR, os.str());
        ret = false;
        continue;
      }
      if (!regionptr1)
      {
        std::ostringstream os; 
        os << "Interface " << interfaceName << " references non-existent region name " << regionName1 << ".\n";
        OutputStream::WriteOut(OutputStream::ERROR, os.str());
        ret = false;
        continue;
      }

      Interface *interfaceptr = dp->GetInterface(interfaceName);
      if (interfaceptr)
      {
        std::ostringstream os; 
        os << "Interface " << interfaceName << " on regions " << regionName0 << " and " << regionName1 << " being instantiated multiple times.\n";
        OutputStream::WriteOut(OutputStream::ERROR, os.str());
        ret = false;
        continue;
      }

      inodes0.clear();
      inodes1.clear();
      for (MeshNodeList_t::const_iterator tnit = mesh_nodes.begin(); tnit != mesh_nodes.end(); ++tnit)
      {
        size_t iindex = tnit->Index();

        if (iindex > maxCoordinateIndex)
        {
          std::ostringstream os; 
          os << "Interface " << interfaceName << " reference coordinate " << iindex << " which does not exist on any region.\n";
          OutputStream::WriteOut(OutputStream::ERROR, os.str());
          ret = false;
          continue;
        }

        ConstNodePtr np0 = nodeList0[iindex];
        ConstNodePtr np1 = nodeList1[iindex];
        if (np0 && np1)
        {
          inodes0.push_back(np0);
          inodes1.push_back(np1);
        }
        else
        {
          std::ostringstream os; 
          if (!np0)
          {
            os << "Interface " << interfaceName << " reference coordinate " << iindex << " not on region " << regionName0 << ".\n";
          }
          if (!np1)
          {
            os << "Interface " << interfaceName << " reference coordinate " << iindex << " not on region " << regionName1 << ".\n";
          }
          OutputStream::WriteOut(OutputStream::ERROR, os.str());
          ret = false;
          continue;
        }
      }

      if (inodes0.empty())
      {
        std::ostringstream os; 
        os << "Interface " << interfaceName << " does not reference any nodes on regions " << regionName0 << " and " << regionName1 << " .\n";
        OutputStream::WriteOut(OutputStream::ERROR, os.str());
        ret = false;
        continue;
      }

      dp->AddInterface(new Interface(interfaceName, regionptr0, regionptr1, inodes0, inodes1));
      std::ostringstream os; 
      os << "Adding interface " << interfaceName << " with " << inodes0.size() << ", " << inodes1.size() << " nodes" << "\n";
      OutputStream::WriteOut(OutputStream::INFO, os.str());
    }
  }


  //// Create the device
  //// Add the device

  //// Add the coordinates
  //// Add the regions
  //// Add the interfaces
  //// Add the contacts
  //// finalize device and region
  //// create the nodes
  //// create the edges
  return ret;
}

///// This function should mostly just do error checking
bool GmshLoader::Finalize_(std::string &errorString)
{
  bool ret = true;
  /// First we need to process the coordinates over the entire device
  /// for each element, we must put it into the appropriate region
  /// Mesh coordinates are over the entire device

  for (size_t i = 0; i < elementList.size(); ++i)
  {
    const GmshElement &elem = elementList[i];

    const size_t physical_number = elem.physical_number;
    const size_t dimension = elem.element_type;
    const Shapes::ElementType_t element_type = elem.element_type;
    std::string physicalName; 
    const PhysicalIndexToName_t &physicalIndexNameMap = physicalDimensionIndexNameMap[dimension];
    PhysicalIndexToName_t::const_iterator pit = physicalIndexNameMap.find(physical_number);
    if (pit != physicalIndexNameMap.end())
    {
      physicalName = pit->second;
    }
    else
    {
      std::ostringstream os;
      os << physical_number;
      physicalName = os.str();
    }

    gmshShapesMap[physicalName].AddShape(element_type, elem.node_indexes);
  }

  
  for (ShapesMap_t::iterator it = gmshShapesMap.begin(); it != gmshShapesMap.end(); ++it)
  {
    Shapes &shapes = it->second;

    if (shapes.GetNumberOfTypes() != 1)
    {
      ret = false;
      std::ostringstream os; 
      os << "Physical group name " << it->first << " has multiple element types.\n";
      OutputStream::WriteOut(OutputStream::ERROR, os.str());
    }
    else if (shapes.GetDimension() > dimension)
    {
      dimension = shapes.GetDimension();
    }
    shapes.DecomposeAndUniquify();

    {
    std::ostringstream os; 
    os << "Physical group name " << it->first << " has " << shapes.Tetrahedra.size() << " Tetrahedra.\n";
    os << "Physical group name " << it->first << " has " << shapes.Triangles.size() << " Triangles.\n";
    os << "Physical group name " << it->first << " has " << shapes.Lines.size() << " Lines.\n";
    os << "Physical group name " << it->first << " has " << shapes.Points.size() << " Points.\n";
    OutputStream::WriteOut(OutputStream::INFO, os.str());
    }
  }

  errorString += "Check log for errors.\n";
  if (ret)
  {
      this->SetFinalized();
  }
  return ret;
}

bool GmshLoader::HasPhysicalName(const size_t d, const size_t i) const
{
  bool ret = false;
  const PhysicalIndexToName_t &physicalIndexNameMap = physicalDimensionIndexNameMap[d];
  PhysicalIndexToName_t::const_iterator it = physicalIndexNameMap.find(i);
  if (it != physicalIndexNameMap.end())
  {
    ret = !((it->second).empty());
  }

  return ret;
}
}

