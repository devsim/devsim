/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "DevsimRestartWriter.hh"
#include "GlobalData.hh"
#include "Device.hh"
#include "Coordinate.hh"
#include "Region.hh"
#include "Node.hh"
#include "Edge.hh"
#include "Triangle.hh"
#include "Tetrahedron.hh"
#include "Contact.hh"
#include "Interface.hh"
#include "NodeModel.hh"
#include "EdgeModel.hh"
#include "TriangleEdgeModel.hh"
#include "TetrahedronEdgeModel.hh"
#include "dsAssert.hh"
#include "InterfaceNodeModel.hh"
#include "EquationHolder.hh"
#include "ContactEquationHolder.hh"
#include "InterfaceEquationHolder.hh"
#include <sstream>
#include <fstream>
#include <iomanip>

namespace {
void WriteCoordinates(std::ostream &myfile, const Device::CoordinateList_t &clist)
{
    myfile << "begin_coordinates\n";
    for (Device::CoordinateList_t::const_iterator cit = clist.begin(); cit != clist.end(); ++cit)
    {
        const Vector<double> &pos = (*cit)->Position();
        myfile << pos.Getx() << "\t" << pos.Gety() << "\t" << pos.Getz() << "\n";
    }
    myfile << "end_coordinates\n\n";
}

void WriteNodes(std::ostream &myfile, const ConstNodeList &nlist)
{
    myfile << "begin_nodes\n";
    for (ConstNodeList::const_iterator nit = nlist.begin(); nit != nlist.end(); ++nit)
    {
        const Node &n = **nit;
        const Coordinate &c = n.GetCoordinate();
        const size_t index = c.GetIndex();
        myfile << index << "\n";
    }
    myfile << "end_nodes\n\n";
}

void WriteEdges(std::ostream &myfile, const ConstEdgeList &elist)
{
    myfile << "begin_edges\n";
    for (ConstEdgeList::const_iterator eit = elist.begin(); eit != elist.end(); ++eit)
    {
        const ConstNodeList &nlist = (*eit)->GetNodeList();
        myfile << nlist[0]->GetIndex() << "\t" << nlist[1]->GetIndex() << "\n";
    }
    myfile << "end_edges\n\n";
}

void WriteTriangles(std::ostream &myfile, const ConstTriangleList &tlist)
{
    if (!tlist.empty())
    {
        myfile << "begin_triangles\n";
        for (ConstTriangleList::const_iterator tit = tlist.begin(); tit != tlist.end(); ++tit)
        {
            const ConstNodeList &nlist = (*tit)->GetNodeList();
            myfile << nlist[0]->GetIndex() << "\t" << nlist[1]->GetIndex() << "\t" << nlist[2]->GetIndex() << "\n";
        }
        myfile << "end_triangles\n\n";
    }
}

void WriteTetrahedra(std::ostream &myfile, const ConstTetrahedronList &tlist)
{
    if (!tlist.empty())
    {
        myfile << "begin_tetrahedra\n";
        for (ConstTetrahedronList::const_iterator tit = tlist.begin(); tit != tlist.end(); ++tit)
        {
            const ConstNodeList &nlist = (*tit)->GetNodeList();
            myfile << nlist[0]->GetIndex() << "\t" << nlist[1]->GetIndex() << "\t" << nlist[2]->GetIndex() << "\t" << nlist[3]->GetIndex() << "\n";
        }
        myfile << "end_tetrahedra\n\n";
    }
}

void WriteEquations(std::ostream &myfile, const EquationPtrMap_t &eqlist)
{
  for (EquationPtrMap_t::const_iterator eit = eqlist.begin(); eit != eqlist.end(); ++eit)
  {
    (eit->second).DevsimSerialize(myfile);
  }
}

void WriteContactEquations(std::ostream &myfile, const ContactEquationPtrMap_t &eqlist)
{
  for (ContactEquationPtrMap_t::const_iterator eit = eqlist.begin(); eit != eqlist.end(); ++eit)
  {
    (eit->second).DevsimSerialize(myfile);
  }
}

void WriteInterfaceEquations(std::ostream &myfile, const InterfaceEquationPtrMap_t &eqlist)
{
  for (InterfaceEquationPtrMap_t::const_iterator eit = eqlist.begin(); eit != eqlist.end(); ++eit)
  {
    (eit->second).DevsimSerialize(myfile);
  }
}

void WriteNodeModels(std::ostream &myfile, const Region::NodeModelList_t &nmlist)
{
  for (Region::NodeModelList_t::const_iterator nit = nmlist.begin(); nit != nmlist.end(); ++nit)
  {
    (nit->second)->DevsimSerialize(myfile);
  }
}

void WriteEdgeModels(std::ostream &myfile, const Region::EdgeModelList_t &emlist)
{
  for (Region::EdgeModelList_t::const_iterator nit = emlist.begin(); nit != emlist.end(); ++nit)
  {
    (nit->second)->DevsimSerialize(myfile);
  }
}

void WriteTriangleEdgeModels(std::ostream &myfile, const Region::TriangleEdgeModelList_t &emlist)
{
  for (Region::TriangleEdgeModelList_t::const_iterator nit = emlist.begin(); nit != emlist.end(); ++nit)
  {
    (nit->second)->DevsimSerialize(myfile);
  }
}

void WriteTetrahedronEdgeModels(std::ostream &myfile, const Region::TetrahedronEdgeModelList_t &emlist)
{
  for (Region::TetrahedronEdgeModelList_t::const_iterator nit = emlist.begin(); nit != emlist.end(); ++nit)
  {
    (nit->second)->DevsimSerialize(myfile);
  }
}

#if 0
void WriteInterfaceNodeModelHeader(std::ostream &myfile, const Interface::NameToInterfaceNodeModelMap_t &imlist)
{
  myfile << "begin_interface_node_models\n";
  for (Interface::NameToInterfaceNodeModelMap_t::const_iterator nit = imlist.begin(); nit != imlist.end(); ++nit)
  {
    const std::string &name = nit->first;
    myfile << "\"" << name << "\"\n";
  }
  myfile << "end_interface_node_models\n\n";
}
#endif

void WriteInterfaceNodeModels(std::ostream &myfile, const Interface::NameToInterfaceNodeModelMap_t &imlist)
{
  for (Interface::NameToInterfaceNodeModelMap_t::const_iterator nit = imlist.begin(); nit != imlist.end(); ++nit)
  {
    (nit->second)->DevsimSerialize(myfile);
  }
}


bool WriteSingleDevice(const std::string &dname, std::ostream &myfile, std::string &errorString)
{
  bool ret = true;
  std::ostringstream os;

  std::ios oldState(nullptr);
  oldState.copyfmt(myfile);

  myfile << std::setprecision(15) << std::scientific;
  GlobalData   &gdata = GlobalData::GetInstance();

  DevicePtr dp = gdata.GetDevice(dname);

  if (!dp)
  {
    ret = false;
    os << "ERROR: Device \"" << dname << "\" does not exist\n";
  }
  else
  {
    Device &dev = *dp;
    const size_t dimension = dev.GetDimension();

    myfile << "begin_device \"" << dname << "\"\n";

    {
        const Device::CoordinateList_t &clist = dev.GetCoordinateList();
        WriteCoordinates(myfile, clist);
    }

    const Device::RegionList_t &rlist = dev.GetRegionList();
    for (Device::RegionList_t::const_iterator rit = rlist.begin(); rit != rlist.end(); ++rit)
    {
      const std::string &rname = rit->first;
      const Region      &reg   = *(rit->second);

      myfile << "begin_region \"" << rname << "\" \"" << reg.GetMaterialName() << "\"\n";

      const ConstNodeList &nlist = reg.GetNodeList();
      WriteNodes(myfile, nlist);

      if (dimension == 1)
      {
        const ConstEdgeList &elist = reg.GetEdgeList();
        WriteEdges(myfile, elist);
      }
      else if (dimension == 2)
      {
        const ConstTriangleList &tlist = reg.GetTriangleList();
        WriteTriangles(myfile, tlist);
      }
      else if (dimension == 3)
      {
        const ConstTetrahedronList &tetrahedron_list = reg.GetTetrahedronList();
        WriteTetrahedra(myfile, tetrahedron_list);
      }

      const Region::NodeModelList_t &nodeModelList = reg.GetNodeModelList();
      const Region::EdgeModelList_t &edgeModelList = reg.GetEdgeModelList();
      const Region::TriangleEdgeModelList_t &triangleEdgeModelList = reg.GetTriangleEdgeModelList();
      const Region::TetrahedronEdgeModelList_t &tetrahedronEdgeModelList = reg.GetTetrahedronEdgeModelList();

#if 0
      WriteNodeModelHeader(myfile, nodeModelList);
      WriteEdgeModelHeader(myfile, edgeModelList);
      WriteTriangleEdgeModelHeader(myfile, triangleEdgeModelList);
      WriteTetrahedronEdgeModelHeader(myfile, tetrahedronEdgeModelList);
#endif


      WriteNodeModels(myfile, nodeModelList);
      WriteEdgeModels(myfile, edgeModelList);
      WriteTriangleEdgeModels(myfile, triangleEdgeModelList);
      WriteTetrahedronEdgeModels(myfile, tetrahedronEdgeModelList);

      const EquationPtrMap_t &equationList = reg.GetEquationPtrList();

      WriteEquations(myfile, equationList);

      myfile << "end_region\n\n";
    }

    const Device::ContactList_t &ctlist = dev.GetContactList();
    for (Device::ContactList_t::const_iterator cit = ctlist.begin(); cit != ctlist.end(); ++cit)
    {
      const std::string &cname = cit->first;
      const Contact     &cnt   = *(cit->second);
      myfile << "begin_contact \"" << cname << "\" \"" << cnt.GetRegion()->GetName() << "\" \"" << cnt.GetMaterialName() << "\"\n";

      const ConstNodeList_t &ctnodes = cnt.GetNodes();

      if (dimension == 1)
      {
        myfile << "begin_nodes\n";
        for (ConstNodeList_t::const_iterator ctit = ctnodes.begin(); ctit != ctnodes.end(); ++ctit)
        {
            myfile << (*ctit)->GetIndex() << "\n";
        }
        myfile << "end_nodes\n\n";
      }
      else if (dimension == 2)
      {
        const ConstEdgeList_t &itedges = cnt.GetEdges();
        if (!itedges.empty())
        {
          myfile << "begin_edges\n";
          for (size_t i = 0; i < itedges.size(); ++i)
          {
            myfile << itedges[i]->GetHead()->GetIndex() << "\t" << itedges[i]->GetTail()->GetIndex() << "\n";
          }
          myfile << "end_edges\n\n";
        }
      }
      else if (dimension == 3)
      {
        const ConstTriangleList_t &ittriangles = cnt.GetTriangles();
        if (!ittriangles.empty())
        {
          myfile << "begin_triangles\n";

          for (size_t i = 0; i < ittriangles.size(); ++i)
          {
            const std::vector<ConstNodePtr> &triangle_nodes = ittriangles[i]->GetNodeList();
            myfile << triangle_nodes[0]->GetIndex() << "\t" << triangle_nodes[1]->GetIndex() << "\t" << triangle_nodes[2]->GetIndex() << "\n";

          }
          myfile << "end_triangles\n\n";
        }
      }

/*
      const Region &reg = *(cnt.GetRegion());
*/

      const ContactEquationPtrMap_t &contact_equations = cnt.GetEquationPtrList();

      WriteContactEquations(myfile, contact_equations);

      myfile << "end_contact\n\n";
    }

    const Device::InterfaceList_t &itlist = dev.GetInterfaceList();
    for (Device::InterfaceList_t::const_iterator iit = itlist.begin(); iit != itlist.end(); ++iit)
    {
      const std::string &iname = iit->first;
      const Interface   &iint   = *(iit->second);
      myfile << "begin_interface \"" << iname << "\" \"" << iint.GetRegion0()->GetName() << "\" \"" << iint.GetRegion1()->GetName() << "\"\n";

      const ConstNodeList_t &itnodes0 = iint.GetNodes0();
      const ConstNodeList_t &itnodes1 = iint.GetNodes1();
      dsAssert(itnodes0.size() == itnodes1.size(), "UNEXPECTED");

      if (dimension == 1 || (!iint.ElementsProvided()))
      {
        myfile << "begin_nodes\n";
        for (size_t i = 0; i < itnodes0.size(); ++i)
        {
          myfile << itnodes0[i]->GetIndex() << "\t" << itnodes1[i]->GetIndex() << "\n";
        }
        myfile << "end_nodes\n\n";
      }
      else if (dimension == 2)
      {
        const ConstEdgeList_t &itedges0 = iint.GetEdges0();
        const ConstEdgeList_t &itedges1 = iint.GetEdges1();
        if (!itedges0.empty() && (itedges0.size() == itedges1.size()))
        {
          myfile << "begin_edges\n";
          for (size_t i = 0; i < itedges0.size(); ++i)
          {
            myfile << itedges0[i]->GetHead()->GetIndex() << "\t" << itedges0[i]->GetTail()->GetIndex() << "\t"
                   << itedges1[i]->GetHead()->GetIndex() << "\t" << itedges1[i]->GetTail()->GetIndex() << "\n";
          }
          myfile << "end_edges\n\n";
        }
      }
      else if (dimension == 3)
      {
        const ConstTriangleList_t &ittriangles0 = iint.GetTriangles0();
        const ConstTriangleList_t &ittriangles1 = iint.GetTriangles1();
        if (!ittriangles0.empty() && (ittriangles0.size() == ittriangles1.size()))
        {
          myfile << "begin_triangles\n";

          for (size_t i = 0; i < ittriangles0.size(); ++i)
          {
            const std::vector<ConstNodePtr> &triangle0_nodes = ittriangles0[i]->GetNodeList();
            const std::vector<ConstNodePtr> &triangle1_nodes = ittriangles1[i]->GetNodeList();
            myfile << triangle0_nodes[0]->GetIndex() << "\t" << triangle0_nodes[1]->GetIndex() << "\t" << triangle0_nodes[2]->GetIndex() << "\t"
                   << triangle1_nodes[0]->GetIndex() << "\t" << triangle1_nodes[1]->GetIndex() << "\t" << triangle1_nodes[2]->GetIndex() << "\n";

          }
          myfile << "end_triangles\n\n";
        }
      }



      const Interface::NameToInterfaceNodeModelMap_t &imlist = iint.GetInterfaceNodeModelList();

#if 0
      WriteInterfaceNodeModelHeader(myfile, imlist);
#endif
      WriteInterfaceNodeModels(myfile, imlist);

      InterfaceEquationPtrMap_t interface_equation_list = iint.GetInterfaceEquationList();

      WriteInterfaceEquations(myfile, interface_equation_list);

      myfile << "end_interface\n\n";
    }

      myfile << "end_device\n\n";

  }
  myfile.copyfmt(oldState);

  errorString += os.str();
  return ret;

}
}

DevsimRestartWriter::~DevsimRestartWriter()
{
}

bool DevsimRestartWriter::WriteMesh_(const std::string &deviceName, const std::string &filename, std::string &errorString)
{
    bool ret = true;
    std::ostringstream os;

    std::ofstream myfile;
    myfile.open (filename.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if (myfile.bad())
    {
        ret = false;
        os << "Could not open \"" << filename << "\" for writing\n";
    }
    else
    {
        ret = WriteSingleDevice(deviceName, myfile, errorString);
    }
    errorString += os.str();
    return ret;
}

bool DevsimRestartWriter::WriteMeshes_(const std::string &filename, std::string &errorString)
{
    bool ret = true;
    std::ostringstream os;

    std::ofstream myfile;
    myfile.open (filename.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if (myfile.bad())
    {
        ret = false;
        os << "Could not open \"" << filename << "\" for writing\n";
    }

    GlobalData   &gdata = GlobalData::GetInstance();
    const GlobalData::DeviceList_t &dlist = gdata.GetDeviceList();
    for (GlobalData::DeviceList_t::const_iterator dit = dlist.begin(); dit != dlist.end(); ++dit)
    {
        const std::string &dname = dit->first;
        ret = WriteSingleDevice(dname, myfile, errorString);
    }

    myfile.close();

    errorString += os.str();
    return ret;
}

