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

#include "GeniusLoader.hh"
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
#include "ObjectHolder.hh"
#include "NodeSolution.hh"

#include <sstream>

namespace dsMesh {
namespace {
/* local node index are 1 based in cgns */
void processNodes(const MeshNodeList_t &mnlist, const std::vector<Coordinate *> &clist, std::vector<Node *> &node_list)
{

  node_list.clear();
  node_list.resize(mnlist.size()+1);

  for (size_t node_index = 0; node_index < mnlist.size(); ++node_index)
  {
    const MeshNode &mesh_node = mnlist[node_index];
    const size_t cindex = mesh_node.Index();

    dsAssert(cindex < clist.size(), "UNEXPECTED");

    dsAssert(clist[cindex] != 0, "UNEXPECTED");

    Node *tp = new Node(node_index, clist[cindex]);
    node_list[node_index+1] = tp;
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

GeniusLoader::GeniusLoader(const std::string &n) : Mesh(n), dimension(0), resistive_metal(false)
{
    //// Arbitrary number
    meshCoordinateList.reserve(10000);
}

GeniusLoader::~GeniusLoader() {
}

void GeniusLoader::AddRegion(GeniusRegionPtr grip)
{
  const std::string &region_name = grip->region;
  geniusRegionMap[region_name] = grip;
  GeniusRegion &ri = *grip;
  std::vector<double> &x = ri.vec_x;
  std::vector<double> &y = ri.vec_y;
  std::vector<double> &z = ri.vec_z;
  std::vector<int>    &ni = ri.global_node_index;
  for (size_t i = 0; i < x.size(); ++i)
  {
    AddCoordinate(ni[i], MeshCoordinate(x[i], y[i], z[i]));
  }
  std::vector<double>(0).swap(x);
  std::vector<double>(0).swap(y);
  std::vector<double>(0).swap(z);


  std::vector<GeniusBoundaryPtr> &bocos = ri.bocos;
  for (size_t i = 0; i < bocos.size(); ++i)
  {
    GeniusBoundaryPtr bip = bocos[i];
    if (bip->label.empty())
    {
      geniusBoundaryMap[bip->name].push_back(bip);
    }
    else
    {
      geniusBoundaryMap[bip->label].push_back(bip);
    }
  }
}

std::vector<int> *GeniusLoader::GetGeniusRegionBoundaryPoints(const std::string &genius_region_name, const std::string &genius_boundary_name)
{
  std::vector<int> *ret = nullptr;

  do
  {
    if (!HasGeniusRegion(genius_region_name))
    {
      std::ostringstream os; 
      os << "Cannot find genius region " << genius_region_name << ".\n";
      OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
      ret = nullptr;
      break;
    }

    GeniusRegion &genius_region = GetGeniusRegion(genius_region_name);

    if (!genius_region.HasBoundary(genius_boundary_name))
    {
      std::ostringstream os; 
      os << "Cannot find genius boundary " << genius_boundary_name << " on genius region.\n";
      OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
      break;
    }

    GeniusBoundary &genius_boundary = genius_region.GetBoundary(genius_boundary_name);

    if (genius_boundary.points.empty())
    {
      std::ostringstream os; 
      os << "Cannot find points genius boundary " << genius_boundary_name << " on genius region " << genius_region_name << ".\n";
      OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
      break;
    }
    else
    {
      ret = &(genius_boundary.points);
    }
  } while(0);


  return ret;
}

bool GeniusLoader::Instantiate_(const std::string &deviceName, std::string &errorString)
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

  Device::CoordinateList_t coordinate_list(meshCoordinateList.size());

  for (size_t i = 0; i < meshCoordinateList.size(); ++i)
  {
    const MeshCoordinate &mc = (meshCoordinateList[i]);
    CoordinatePtr new_coordinate =  new Coordinate(mc.GetX(), mc.GetY(), mc.GetZ()); 
    coordinate_list[i] = new_coordinate;
    dp->AddCoordinate(new_coordinate);
  }
  {
    std::ostringstream os; 
    os << "Device " << deviceName << " has " << meshCoordinateList.size() << " coordinates.\n";
    OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
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

      tetrahedronList.clear();
      triangleList.clear();
      edgeList.clear();

      const std::string &regionName = rit->first;
      const GeniusRegionInfo &rinfo = rit->second;
      const std::string &genius_region_name = rinfo.genius_region_name;



      Region *regionptr = dp->GetRegion(regionName);
      if (!regionptr)
      {
        regionptr = new Region(regionName, rinfo.material, dimension, dp);
        dp->AddRegion(regionptr);
      }

      Region &region = *regionptr;

      if (!HasGeniusRegion(genius_region_name))
      {
        std::ostringstream os; 
        os << "Cannot find genius region " << genius_region_name << " for region " << regionName << "\n";
        OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
        ret = false;
        continue;
      }

      GeniusRegion &genius_region = GetGeniusRegion(genius_region_name);

      const std::vector<int> &gni = genius_region.global_node_index;
      mesh_nodes.reserve(gni.size());
      //// mesh_nodes reference global coordinate index through gni
      for (size_t i = 0; i < gni.size(); ++i)
      {
        mesh_nodes.push_back(MeshNode(gni[i]));
      }

      {
        std::ostringstream os; 
        os << "Region " << regionName << " has " << mesh_nodes.size() << " nodes.\n";
        OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
      }

      std::vector<NodePtr> &nodeList = RegionNameToNodeMap[regionName];

      processNodes(mesh_nodes, coordinate_list, nodeList);

      for (size_t i = 1; i < nodeList.size(); ++i)
      {
        region.AddNode(nodeList[i]);
      }

      {
        std::ostringstream os; 
        os << "Region " << regionName << " has " << nodeList.size() << " nodes.\n";
        OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
      }

      if (dimension == 3)
      {
        processTetrahedra(geniusShapesMap[regionName].Tetrahedra, nodeList, tetrahedronList);
        region.AddTetrahedronList(tetrahedronList);
      }

      if (dimension >= 2)
      {
        processTriangles(geniusShapesMap[regionName].Triangles, nodeList, triangleList);
        region.AddTriangleList(triangleList);
      }
      processEdges(geniusShapesMap[regionName].Lines, nodeList, edgeList);
      region.AddEdgeList(edgeList);
      region.FinalizeMesh();
      CreateDefaultModels(&region);

      for (std::map<std::string, std::vector<double> >::iterator sit = genius_region.solutions.begin(); sit != genius_region.solutions.end(); ++sit)
      {
        const std::string &sname = sit->first;
        auto nodesol = CreateNodeSolution(sname, &region);

        std::vector<double> &nval = sit->second;
        const size_t nlen = nval.size();
        bool is_uniform = true;
        const double val0 = nval[0];
        for (size_t i = 1; i < nlen; ++i)
        {
          if (val0 != nval[i])
          {
            is_uniform = false;
            break;
          }
        }
        if (is_uniform)
        {
          nodesol->SetValues(val0);
        }
        else
        {
          nodesol->SetValues(nval);
        }
      }

      {
        for (std::map<std::string, std::string>::iterator sit = genius_region.units.begin(); sit != genius_region.units.end(); ++sit)
        {
          const std::string &sname = sit->first;
          gdata.AddDBEntryOnRegion(deviceName, regionName, sname, ObjectHolder(sit->second));
        }
      }
    }

  }
  //// Now process the contact
  {
    ConstNodeList cnodes;
    for (MapToContactInfo_t::const_iterator cit = contactMap.begin(); cit != contactMap.end(); ++cit)
    {
      const std::string &contactName = cit->first;
      const GeniusContactInfo &cinfo = cit->second;

      const std::string &regionName = cinfo.region;
      const std::string &materialName = cinfo.material;

      if ((!RegionNameToNodeMap.count(regionName)) || (!HasRegionInfo(regionName)))
      {
        std::ostringstream os; 
        os << "Contact " << contactName << " references non-existent region name " << regionName << ".\n";
        OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
        ret = false;
        continue;
      }

      GeniusRegionInfo &rinfo = GetRegionInfo(regionName);
      const std::string &genius_region_name = rinfo.genius_region_name;



      /** these are the 1 based nodes on the Map */
      const std::vector<NodePtr> &nodeList = RegionNameToNodeMap[regionName];

      /// This is the genius name of the boundary condition
      const std::string &genius_boundary_name = cinfo.boundary_name;


      Region *regionptr = dp->GetRegion(regionName);
      if (!regionptr)
      {
        std::ostringstream os; 
        os << "Contact " << contactName << " references non-existent region name " << regionName << ".\n";
        OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
        ret = false;
        continue;
      }

      Contact *contactptr = dp->GetContact(contactName);
      if (contactptr)
      {
        std::ostringstream os; 
        os << "Contact " << contactName << " on region " << regionName << " being instantiated multiple times.\n";
        OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
        ret = false;
        continue;
      }

      {
        std::vector<int> *pp = GetGeniusRegionBoundaryPoints(genius_region_name, genius_boundary_name);
        if (!pp)
        {
          ret = false;
          continue;
        }
        cnodes.clear();
        std::vector<int> &points = *pp;
        const size_t plen = points.size();
        const size_t nlen = nodeList.size();
        dsAssert ((plen > 0), "UNEXPECTED mesh problem");
        for (size_t i = 0; i < plen; ++i)
        {
          size_t pindex = static_cast<size_t>(points[i]);
          dsAssert ((pindex > 1) || (pindex < nlen), "UNEXPECTED mesh problem");
          ConstNodePtr np = nodeList[pindex];
          cnodes.push_back(np);
        }
      }
    
      dp->AddContact(new Contact(contactName, regionptr, cnodes, materialName));
      std::ostringstream os; 
      os << "Contact " << contactName << " in region " << regionName << " with " << cnodes.size() << " nodes" << "\n";
      OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
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
      const GeniusInterfaceInfo &iinfo   = iit->second;

      const std::string &regionName0 = iinfo.region0;
      const std::string &regionName1 = iinfo.region1;

      const std::string &genius_boundary_name = iinfo.boundary_name;

      if ((!RegionNameToNodeMap.count(regionName0)) || (!HasRegionInfo(regionName0)))
      {
        std::ostringstream os; 
        os << "Interface " << interfaceName << " references non-existent region name " << regionName0 << ".\n";
        OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
        ret = false;
        continue;
      }

      if ((!RegionNameToNodeMap.count(regionName1)) || (!HasRegionInfo(regionName1)))
      {
        std::ostringstream os; 
        os << "Interface " << interfaceName << " references non-existent region name " << regionName1 << ".\n";
        OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
        ret = false;
        continue;
      }

      GeniusRegionInfo &rinfo0 = GetRegionInfo(regionName0);
      GeniusRegionInfo &rinfo1 = GetRegionInfo(regionName1);


      const std::vector<NodePtr> &nodeList0 = RegionNameToNodeMap[regionName0];
      const std::vector<NodePtr> &nodeList1 = RegionNameToNodeMap[regionName1];


      Region *regionptr0 = dp->GetRegion(regionName0);
      Region *regionptr1 = dp->GetRegion(regionName1);
      if (!regionptr0)
      {
        std::ostringstream os; 
        os << "Interface " << interfaceName << " references non-existent region name " << regionName0 << ".\n";
        OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
        ret = false;
        continue;
      }
      if (!regionptr1)
      {
        std::ostringstream os; 
        os << "Interface " << interfaceName << " references non-existent region name " << regionName1 << ".\n";
        OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
        ret = false;
        continue;
      }

      Interface *interfaceptr = dp->GetInterface(interfaceName);
      if (interfaceptr)
      {
        std::ostringstream os; 
        os << "Interface " << interfaceName << " on regions " << regionName0 << " and " << regionName1 << " being instantiated multiple times.\n";
        OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
        ret = false;
        continue;
      }

      const std::string &genius_region_name0 = rinfo0.genius_region_name;
      const std::string &genius_region_name1 = rinfo1.genius_region_name;
      std::vector<int> *pp0 = GetGeniusRegionBoundaryPoints(genius_region_name0, genius_boundary_name);
      std::vector<int> *pp1 = GetGeniusRegionBoundaryPoints(genius_region_name1, genius_boundary_name);
      if (!(pp0 && pp1))
      {
        ret = false;
        continue;
      }


      std::vector<int> &points0 = *pp0;
      std::vector<int> &points1 = *pp1;

      if (points0.size() != points1.size())
      {
        std::ostringstream os; 
        os << "Interface " << interfaceName << " on regions " << regionName0 << " and " << regionName1 << " has points mismatch.\n";
        OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
        ret = false;
        continue;
      }

      const size_t plen = points0.size();
      const size_t nlen0 = nodeList0.size();
      const size_t nlen1 = nodeList1.size();
      dsAssert ((plen > 0), "UNEXPECTED mesh problem");
      inodes0.clear();
      inodes1.clear();
      for (size_t i = 0; i < plen; ++i)
      {
        const size_t pindex0 = static_cast<size_t>(points0[i]);
        const size_t pindex1 = static_cast<size_t>(points1[i]);
        dsAssert ((pindex0 > 1) || (pindex0 < nlen0), "UNEXPECTED mesh problem");
        dsAssert ((pindex1 > 1) || (pindex1 < nlen1), "UNEXPECTED mesh problem");

        ConstNodePtr np0 = nodeList0[pindex0];
        ConstNodePtr np1 = nodeList1[pindex1];

        inodes0.push_back(np0);
        inodes1.push_back(np1);
      }

      dp->AddInterface(new Interface(interfaceName, regionptr0, regionptr1, inodes0, inodes1));
      std::ostringstream os; 
      os << "Adding interface " << interfaceName << " with " << inodes0.size() << ", " << inodes1.size() << " nodes" << "\n";
      OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
    }
  }

  gdata.AddDBEntryOnDevice(deviceName, "magic_number", ObjectHolder(GetMagicNumber()));
  //// TODO: input scaling factor?
  return ret;
}

///// This function should mostly just do error checking
bool GeniusLoader::Finalize_(std::string &errorString)
{
  bool ret = true;
  /// Mesh coordinates are over the entire device


  {
    for (MapToRegionInfo_t::iterator it = regionMap.begin(); it != regionMap.end(); ++it)
    {
      const std::string &region_name = it->first;
      const std::string &genius_region_name = it->second.genius_region_name;

      Shapes &region_shapes = geniusShapesMap[region_name];

      MapToGeniusRegion_t::iterator jit = geniusRegionMap.find(genius_region_name);
      if (jit != geniusRegionMap.end())
      {
        region_shapes.AddShapes(jit->second->genius_shapes);
      }
      else
      {
        ret = false;
        std::ostringstream os; 
        os << "Genius region name " << genius_region_name << " does not exist.\n";
        OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
      }
    }
  }

  {
    for (MapToContactInfo_t::iterator it = contactMap.begin(); it != contactMap.end(); ++it)
    {
      const std::string &contact_name = it->first;
      const std::string &region_name = it->second.region;
      const std::string &boco_name = it->second.boundary_name;
      MapToRegionInfo_t::iterator rit = regionMap.find(region_name);
      if (rit == regionMap.end())
      {
        ret = false;
        std::ostringstream os; 
        os << "Region name " << region_name << " does not exist for contact " << contact_name << ".\n";
        OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
      }
      else
      {
        bool boco_found = false;
        const std::string &genius_region_name =  rit->second.genius_region_name;

        GeniusRegion &genius_region = *(geniusRegionMap[genius_region_name]);
        boco_found = genius_region.HasBoundary(boco_name);
        if (!boco_found)
        {
            ret = false;
            std::ostringstream os; 
            os << "Region name " << region_name << " does not have contact " << contact_name << ".\n";
            OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
        }
      }
    }
  }

  {
    for (MapToInterfaceInfo_t::iterator it = interfaceMap.begin(); it != interfaceMap.end(); ++it)
    {
      const std::string &interface_name = it->first;
      const std::string &region0_name = it->second.region0;
      const std::string &region1_name = it->second.region1;
      const std::string &boco_name = it->second.boundary_name;
      MapToRegionInfo_t::iterator rit0 = regionMap.find(region0_name);
      MapToRegionInfo_t::iterator rit1 = regionMap.find(region1_name);
      if (rit0 == regionMap.end())
      {
        ret = false;
        std::ostringstream os; 
        os << "Region name " << region0_name << " does not exist for interface " << interface_name << ".\n";
        OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
      }
      if (rit1 == regionMap.end())
      {
        ret = false;
        std::ostringstream os; 
        os << "Region name " << region1_name << " does not exist for interface " << interface_name << ".\n";
        OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
      }

      if (ret)
      {
        const std::string &genius_region0_name =  rit0->second.genius_region_name;
        const std::string &genius_region1_name =  rit1->second.genius_region_name;

        GeniusRegion &genius_region0 = *(geniusRegionMap[genius_region0_name]);
        GeniusRegion &genius_region1 = *(geniusRegionMap[genius_region1_name]);

        bool boco0_found = genius_region0.HasBoundary(boco_name);
        if (!boco0_found)
        {
            ret = false;
            std::ostringstream os; 
            os << "Region name " << region0_name << " does not have interface " << interface_name << ".\n";
            OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
        }

        bool boco1_found = genius_region0.HasBoundary(boco_name);
        if (!boco1_found)
        {
            ret = false;
            std::ostringstream os; 
            os << "Region name " << region1_name << " does not have interface " << interface_name << ".\n";
            OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
        }

        if (ret)
        {
          if (genius_region0.GetBoundary(boco_name).points.size() != genius_region1.GetBoundary(boco_name).points.size())
          {
            ret = false;
            std::ostringstream os; 
            os << "Regions " << region0_name << " and " << region1_name << " have point mismatch for interface " << interface_name << ".\n";
            OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
          }
        }
      }
    }
  }
  
  for (ShapesMap_t::iterator it = geniusShapesMap.begin(); it != geniusShapesMap.end(); ++it)
  {
    Shapes &shapes = it->second;

    if (shapes.GetNumberOfTypes() != 1)
    {
      ret = false;
      std::ostringstream os; 
      os << "Region name " << it->first << " has multiple element types.\n";
      OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
    }
    else if (shapes.GetDimension() > dimension)
    {
      dimension = shapes.GetDimension();
    }
    shapes.DecomposeAndUniquify();

    {
    std::ostringstream os; 
    os << "Region name " << it->first << " has " << shapes.Tetrahedra.size() << " Tetrahedra.\n";
    os << "Region name " << it->first << " has " << shapes.Triangles.size() << " Triangles.\n";
    os << "Region name " << it->first << " has " << shapes.Lines.size() << " Lines.\n";
    os << "Region name " << it->first << " has " << shapes.Points.size() << " Points.\n";
    OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
    }
  }


  errorString += "Check log for errors.\n";
  if (ret)
  {
      this->SetFinalized();
  }
  return ret;
}

ObjectHolder GeniusLoader::GetMeshInfo()
{
  ObjectHolderMap_t ret;

 /* RESISTIVE */
  ret["resistive"] = ObjectHolder(GetResistive());
  ret["dimension"] = ObjectHolder(static_cast<int>(GetDimension()));

  std::map<std::string, std::set<std::string> > boundary_to_region;
  ObjectHolderMap_t regions;
  ObjectHolderList_t solutions;
  for (MapToGeniusRegion_t::iterator rit = geniusRegionMap.begin(); rit != geniusRegionMap.end(); ++rit)
  {
    const std::string &region_name = rit->first;
    GeniusRegion  &genius_region = *(rit->second);

    ObjectHolderMap_t region_map;
    region_map["material"] = ObjectHolder(genius_region.material);

    ObjectHolderMap_t boundary_map;
    for (std::vector<GeniusBoundaryPtr>::iterator bit = genius_region.bocos.begin(); bit != genius_region.bocos.end(); ++bit)
    {
      std::string name = (*bit)->name;
      if (!(*bit)->label.empty())
      {
        name  = (*bit)->label;
      }

      boundary_map[name] = ObjectHolder((*bit)->has_electrode);
      boundary_to_region[name].insert(region_name);
    }
    region_map["boundary_info"] = ObjectHolder(boundary_map);

    /* map keys are automatically sorted */
    solutions.clear();
    for (std::map<std::string, std::vector<double> >::iterator sit = genius_region.solutions.begin(); sit != genius_region.solutions.end(); ++sit)
    {
      solutions.push_back(ObjectHolder(sit->first));
    }

    region_map["solutions"] = ObjectHolder(solutions);
    regions[region_name] = ObjectHolder(region_map);
  }
  ret["regions"] = ObjectHolder(regions);


  ObjectHolderMap_t bmap;
  ObjectHolderList_t ohl;
  for (std::map<std::string, std::set<std::string> >::iterator it = boundary_to_region.begin(); it != boundary_to_region.end(); ++it)
  {
    ohl.clear();
    std::set<std::string> &region_names = it->second;
    for (std::set<std::string>::iterator jt = region_names.begin(); jt != region_names.end(); ++jt)
    {
      ohl.push_back(ObjectHolder(*jt));
    }
    bmap[it->first] = ObjectHolder(ohl);
  }
  ret["boundaries"] = ObjectHolder(bmap);


  return ObjectHolder(ret);
}

bool GeniusRegion::HasBoundary(const std::string &boundary) const
{
  bool ret = false;
  for (size_t i = 0; i < bocos.size(); ++i)
  {
    if (bocos[i]->label == boundary)
    {
      ret = true;
      break;
    }
    else if (bocos[i]->name == boundary)
    {
      ret = true;
      break;
    }
  }
  return ret;
}

GeniusBoundary &GeniusRegion::GetBoundary(const std::string &name)
{
  // boundary not existing is not defined
  size_t index = size_t(-1);
  for (size_t i = 0; i < bocos.size(); ++i)
  {
    if (bocos[i]->label == name)
    {
      index = i;
      break;
    }
    else if (bocos[i]->name == name)
    {
      index = i;
      break;
    }
  }
  return *(bocos[index]);
}

}

