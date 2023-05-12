/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "DevsimWriter.hh"
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
#include <sstream>
#include <fstream>
#include "dsAssert.hh"
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


void WriteNodeModels(std::ostream &myfile, const Region::NodeModelList_t &nmlist)
{
    for (Region::NodeModelList_t::const_iterator nit = nmlist.begin(); nit != nmlist.end(); ++nit)
    {
        const std::string &name = nit->first;
        myfile << "begin_node_solution \"" << name << "\"\n";

        const NodeModel &nmodel = *(nit->second);

        const NodeScalarList<double> &vals = nmodel.GetScalarValues<double>();
        for (NodeScalarList<double>::const_iterator vit = vals.begin(); vit != vals.end(); ++vit)
        {
            myfile << *vit << "\n";
        }
        myfile << "end_node_solution\n";
    }
}

void WriteEdgeModels(std::ostream &myfile, const Region::EdgeModelList_t &emlist)
{
    for (Region::EdgeModelList_t::const_iterator nit = emlist.begin(); nit != emlist.end(); ++nit)
    {
        const std::string &name = nit->first;
        myfile << "begin_edge_solution \"" << name << "\"\n";

        const EdgeModel &emodel = *(nit->second);

        const EdgeScalarList<double> &vals = emodel.GetScalarValues<double>();
        for (EdgeScalarList<double>::const_iterator vit = vals.begin(); vit != vals.end(); ++vit)
        {
            myfile << *vit << "\n";
        }
        myfile << "end_edge_solution\n";
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

            const ConstEdgeList &elist = reg.GetEdgeList();
            WriteEdges(myfile, elist);

            const ConstTriangleList &tlist = reg.GetTriangleList();
            WriteTriangles(myfile, tlist);

            const ConstTetrahedronList &tetrahedron_list = reg.GetTetrahedronList();
            WriteTetrahedra(myfile, tetrahedron_list);

            const Region::NodeModelList_t &nmlist = reg.GetNodeModelList();
            WriteNodeModels(myfile, nmlist);

            const Region::EdgeModelList_t &emlist = reg.GetEdgeModelList();
            WriteEdgeModels(myfile, emlist);

            myfile << "end_region\n\n";
        }

        const Device::ContactList_t &ctlist = dev.GetContactList();
        for (Device::ContactList_t::const_iterator cit = ctlist.begin(); cit != ctlist.end(); ++cit)
        {
            const std::string &cname = cit->first;
            const Contact     &cnt   = *(cit->second);
            myfile << "begin_contact \"" << cname << "\" \"" << cnt.GetRegion()->GetName() << "\" \"" << cnt.GetMaterialName() << "\"\n";

            const ConstNodeList_t &ctnodes = cnt.GetNodes();

            myfile << "begin_nodes\n";
            for (ConstNodeList_t::const_iterator ctit = ctnodes.begin(); ctit != ctnodes.end(); ++ctit)
            {
                myfile << (*ctit)->GetIndex() << "\n";
            }
            myfile << "end_nodes\n";


            myfile << "end_contact\n";
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

            myfile << "begin_nodes\n";
            for (size_t i = 0; i < itnodes0.size(); ++i)
            {
                myfile << itnodes0[i]->GetIndex() << "\t" << itnodes1[i]->GetIndex() << "\n";
            }
            myfile << "end_nodes\n";

            myfile << "end_interface\n";
        }

        myfile << "end_device\n";

    }
    myfile.copyfmt(oldState);

    errorString += os.str();
    return ret;

}
}

DevsimWriter::~DevsimWriter()
{
}

bool DevsimWriter::WriteMesh_(const std::string &deviceName, const std::string &filename, std::string &errorString)
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

bool DevsimWriter::WriteMeshes_(const std::string &filename, std::string &errorString)
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

