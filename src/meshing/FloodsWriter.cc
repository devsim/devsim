/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "FloodsWriter.hh"
#include "GlobalData.hh"
#include "Device.hh"
#include "Coordinate.hh"
#include "Region.hh"
#include "Node.hh"
#include "Edge.hh"
#include "Triangle.hh"
#include "Contact.hh"
#include "Interface.hh"
#include "NodeModel.hh"
#include "EdgeModel.hh"
#include "MeshUtil.hh"
#include <sstream>
#include <fstream>
#include <iomanip>
#include <set>
namespace Floods {
void WriteCoordinates(std::ostream &myfile, const Device::CoordinateList_t &clist)
{
    for (Device::CoordinateList_t::const_iterator cit = clist.begin(); cit != clist.end(); ++cit)
    {
        myfile << "c ";
        const Vector<double> &pos = (*cit)->Position();
        myfile << pos.Getx() << " " << pos.Gety() << " " << pos.Getz() << "\n";
    }
}

void WriteNodesAndSolutions(std::ostream &myfile, const Region &reg, const std::set<std::string> solutions)
{
    std::vector<ConstNodeModelPtr> nmodels;
    for (std::set<std::string>::const_iterator it = solutions.begin(); it != solutions.end(); ++it)
    {
        ConstNodeModelPtr nmp = reg.GetNodeModel(*it);
        nmodels.push_back(nmp);
    }

    const ConstNodeList &cnl = reg.GetNodeList();

    const size_t nvals = cnl.size();

    for (size_t i = 0; i < nvals; ++i)
    {
        myfile << "n " << (cnl[i]->GetCoordinate()).GetIndex();
        for (std::vector<ConstNodeModelPtr>::iterator nmit = nmodels.begin(); nmit != nmodels.end(); ++nmit)
        {
            myfile << " ";
            if (*nmit)
            {
                myfile << (*nmit)->GetScalarValues<double>()[i];
            }
            else
            {
                myfile << "0";
            }
        }
        myfile << "\n";
    }
}

void WriteEdgesAndSolutions(std::ostream &myfile, const Region &reg, const std::set<std::string> solutions)
{
    std::vector<ConstEdgeModelPtr> emodels;
    for (std::set<std::string>::const_iterator it = solutions.begin(); it != solutions.end(); ++it)
    {
        ConstEdgeModelPtr emp = reg.GetEdgeModel(*it);
        emodels.push_back(emp);
    }

    const ConstEdgeList &cel = reg.GetEdgeList();
    const size_t nvals = cel.size();

    const std::vector <Vector<double>> &unitvec = MeshUtil::GetUnitVector(reg);

    for (size_t i = 0; i < nvals; ++i)
    {
        myfile << "e " << (cel[i]->GetHead()->GetCoordinate()).GetIndex()
               << " " << (cel[i]->GetTail()->GetCoordinate()).GetIndex();

        for (std::vector<ConstEdgeModelPtr>::iterator emit = emodels.begin(); emit != emodels.end(); ++emit)
        {
            myfile << " ";
            if (*emit)
            {
                Vector<double> p = unitvec[i];
                p *=  (*emit)->GetScalarValues<double>()[i];
                myfile << p.Getx() << " " << p.Gety() << " " << p.Getz();
            }
            else
            {
                myfile << "0 0 0";
            }
        }
        myfile << "\n";
    }
}

void WriteTriangles(std::ostream &myfile, const Region &reg)
{
    const ConstTriangleList &ctl = reg.GetTriangleList();
    for (ConstTriangleList::const_iterator tit = ctl.begin(); tit != ctl.end(); ++tit)
    {
        myfile << "f3 ";
        const ConstNodeList &nlist = (*tit)->GetNodeList();
        myfile <<  (nlist[0]->GetCoordinate()).GetIndex()
        << " " << (nlist[1]->GetCoordinate()).GetIndex()
        << " " << (nlist[2]->GetCoordinate()).GetIndex()
        << "\n";
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
        os << "ERROR: Device " << dname << " does not exist\n";
    }
    else
    {
        Device &dev = *dp;

        {
            const Device::CoordinateList_t &clist = dev.GetCoordinateList();
            WriteCoordinates(myfile, clist);
        }

        //// write solutions line
        std::set<std::string> nodeModels;
        std::set<std::string> edgeModels;
        {
            const Device::RegionList_t &rlist = dev.GetRegionList();
            for (Device::RegionList_t::const_iterator rit = rlist.begin(); rit != rlist.end(); ++rit)
            {
//              const std::string &rname = rit->first;
                const Region      &reg   = *(rit->second);
                const Region::NodeModelList_t nmlist = reg.GetNodeModelList();
                for (Region::NodeModelList_t::const_iterator nit = nmlist.begin(); nit != nmlist.end(); ++nit)
                {
                    nodeModels.insert(nit->first);
                }

                const Region::EdgeModelList_t emlist = reg.GetEdgeModelList();
                for (Region::EdgeModelList_t::const_iterator eit = emlist.begin(); eit != emlist.end(); ++eit)
                {
                    edgeModels.insert(eit->first);
                }
            }

            myfile << "s";
            for (std::set<std::string>::iterator nmit = nodeModels.begin(); nmit != nodeModels.end(); ++nmit)
            {
                myfile << " " << *nmit;
            }
            myfile << "\n";

            myfile << "se";
            for (std::set<std::string>::iterator emit = edgeModels.begin(); emit != edgeModels.end(); ++emit)
            {
                myfile << " " << *emit;
            }
            myfile << "\n";

            myfile << "sf\nsv\n";
        }


//TODO:  "FINISH FLOODS WRITER"
        const Device::RegionList_t &rlist = dev.GetRegionList();
        for (Device::RegionList_t::const_iterator rit = rlist.begin(); rit != rlist.end(); ++rit)
        {
            const std::string &rname = rit->first;
            const Region      &reg   = *(rit->second);

//TODO:  "fix material id"
            myfile << "r " << "6" << " " << rname << "\n";
//          myfile << "r " << reg.GetMaterialName() << " " << rname << "\n";

            WriteNodesAndSolutions(myfile, reg, nodeModels);

            WriteEdgesAndSolutions(myfile, reg, edgeModels);

            WriteTriangles(myfile, reg);

        }

//TODO:  "interface edges"
//TODO:  "contacts edges"
    }
    // important for postmini
    myfile << "\n";
    myfile.copyfmt(oldState);

    errorString += os.str();
    return ret;
}
}

FloodsWriter::~FloodsWriter()
{
}

bool FloodsWriter::WriteMesh_(const std::string &deviceName, const std::string &filename, std::string &errorString)
{
    bool ret = true;
    std::ostringstream os;

    std::ofstream myfile;
    myfile.open (filename.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if (myfile.bad())
    {
        ret = false;
        os << "Could not open " << filename << " for writing\n";
    }
    else
    {
        ret = Floods::WriteSingleDevice(deviceName, myfile, errorString);
    }
    errorString += os.str();
    return ret;
}

bool FloodsWriter::WriteMeshes_(const std::string &filename, std::string &errorString)
{
    bool ret = true;
    std::ostringstream os;

    std::ofstream myfile;
    myfile.open (filename.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if (myfile.bad())
    {
        ret = false;
        os << "Could not open " << filename << " for writing\n";
    }

    GlobalData   &gdata = GlobalData::GetInstance();
    const GlobalData::DeviceList_t &dlist = gdata.GetDeviceList();

    if (dlist.size() > 1)
    {
        ret = false;
        os << "More than 1 device in simulation when output format only supports one device.\n";
    }
    else
    {
        for (GlobalData::DeviceList_t::const_iterator dit = dlist.begin(); dit != dlist.end(); ++dit)
        {
            const std::string &dname = dit->first;
            ret = Floods::WriteSingleDevice(dname, myfile, errorString);
        }
    }
    myfile.close();

    errorString += os.str();
    return ret;
}
