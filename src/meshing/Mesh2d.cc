/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "Mesh2d.hh"
#include "OutputStream.hh"
#include "dsAssert.hh"
#include "MeshUtil.hh"
#include "DevsimLoader.hh"
#include <sstream>

namespace dsMesh {
Mesh2d::~Mesh2d()
{
}

Mesh2d::Mesh2d(const std::string &nm) : Mesh(nm)
{
}

bool Mesh2d::Instantiate_(const std::string &deviceName, std::string &errorString)
{
    bool ret = true;
    if (meshLoader)
    {
        ret = meshLoader->Instantiate(deviceName, errorString);
    }
    else
    {
        ret = false;
        errorString += "Unable to instantiate " + deviceName + "\n";
    }

    return ret;
}

std::vector<double> Mesh2d::GetLocations(MeshLineVector_t &mlv)
{
    std::vector<double> locations;
    dsAssert(!mlv.empty(), "UNEXPECTED");

    double xl = mlv[0]->getPosition();
    double sl = mlv[0]->getPositiveSpacing();

    locations.push_back(xl);

    double xh;
    double sh;
    for (size_t i = 1; i < mlv.size(); ++i)
    {
        xh  = mlv[i]->getPosition();
        sh  = mlv[i]->getNegativeSpacing();

        /// Get mlv for this section
        MeshUtil::pts_t pts = MeshUtil::getPoints(xl, xh, sl, sh);
        for (size_t j = 1; j < pts.size(); ++j)
        {
            locations.push_back(pts[j]);
        }

        xl = xh;
        sl = mlv[i]->getPositiveSpacing();
    }
    return locations;
}

MeshRegion2dPtr Mesh2d::FindRegion(double x, double y)
{
    MeshRegion2dPtr ret;

    //// TODO: Should use reverse iterator and terminate
    for (std::vector<std::string>::iterator it = regionOrder.begin(); it != regionOrder.end(); ++it)
    {
        if (regions[*it]->IsPointInside(x, y))
        {
            ret = regions[*it];
        }
    }

    return ret;
}

bool Mesh2d::Finalize_(std::string &errorString)
{
    bool ret = true;

    std::vector<MeshCoordinate> coordinates;
//    std::vector<MeshNode>     nodes;
//    std::vector<MeshEdge>       edges;
    std::vector<MeshTriangle>   triangles;

    meshLoader = std::shared_ptr<DevsimLoader>(new DevsimLoader(name));

    //// remove collinear points
    {
        sort(xlines.begin(), xlines.end());
        sort(ylines.begin(), ylines.end());
        MeshLineVector_t nxlines;
        MeshLineVector_t nylines;
        //// Will the unique sort with binary predicate work here???
        size_t xlen = xlines.size();
        size_t ylen = ylines.size();

        if (xlen > 0)
        {
            nxlines.push_back(xlines[0]);
        }
        if (ylen > 0)
        {
            nylines.push_back(ylines[0]);
        }

        std::ostringstream os;
        for (size_t x = 1; x < xlen; ++x)
        {
            if (xlines[x]->getPosition() == xlines[x-1]->getPosition())
            {
                os << "Removing collinear point from meshlines in x direction at " << xlines[x]->getPosition() << "\n";
            }
            else
            {
                nxlines.push_back(xlines[x]);
            }
        }
        for (size_t y = 1; y < ylen; ++y)
        {
            if (ylines[y]->getPosition() == ylines[y-1]->getPosition())
            {
                os << "Removing collinear point from meshlines in y direction at " << ylines[y]->getPosition() << "\n";
            }
            else
            {
                nylines.push_back(ylines[y]);
            }
        }

        xlines.swap(nxlines);
        ylines.swap(nylines);

        std::string warn = os.str();
        if (!warn.empty())
        {
            OutputStream::WriteOut(OutputStream::OutputType::INFO, warn);
        }
    }

    //// Enough meshlines
    {
        if (xlines.size() < 2)
        {
            ret = false;
            errorString += "Must have at least 2 mesh lines in the x direction\n";
        }

        if (ylines.size() < 2)
        {
            ret = false;
            errorString += "Must have at least 2 mesh lines in the y direction\n";
        }

        if (!ret)
        {
            return ret;
        }
    }

    {
        std::vector<double> xlocs = GetLocations(xlines);
        std::vector<double> ylocs = GetLocations(ylines);

        coordinates.clear();
        coordinates.reserve(xlocs.size()*ylocs.size());

        const size_t numx = xlocs.size();
        const size_t numy = ylocs.size();
        /// Integrate the triangulation code later
        for (size_t i = 0 ; i < numx; ++i)
        {
            for (size_t j = 0 ; j < numy; ++j)
            {
                coordinates.push_back(MeshCoordinate(xlocs[i], ylocs[j]));

#if 0
                std::ostringstream os;
                os << "Coord " << xlocs[i] << " " << ylocs[j] << "\n";
                OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str().c_str());
#endif
            }
        }
        meshLoader->AddCoordinates(coordinates);

        for (size_t i = 0 ; i < (numx-1); ++i)
        {
            for (size_t j = 0 ; j < (numy-1); ++j)
            {
                const size_t ul = i * numy + j;
                const size_t ur = (i + 1) * numy + j;
                const size_t ll = i * numy + (j + 1);
                const size_t lr = (i + 1) * numy + (j + 1);

                triangles.push_back(MeshTriangle(ul, ur, ll));
                triangles.push_back(MeshTriangle(ur, lr, ll));
            }
        }
    }


    //// Add triangulation code for arbitrary points here


    /// Process the triangles into regions
    std::vector<size_t> nodesAtCoordinate(coordinates.size(), 0);
    typedef std::map<std::string, std::vector<size_t> > CoordinateToNode_t;
    CoordinateToNode_t CoordinateToNode;
    std::map<std::string, size_t > MaxNodeIndex;
    for (std::vector<std::string>::iterator rt = regionOrder.begin(); rt != regionOrder.end(); ++rt)
    {
        CoordinateToNode[*rt] = std::vector<size_t>(coordinates.size(), size_t(-1));
        MaxNodeIndex[*rt] = 0;
    }

    for (std::vector<MeshTriangle>::iterator it=triangles.begin(); it != triangles.end(); ++it)
    {

        const size_t i0 = it->Index0();
        const size_t i1 = it->Index1();
        const size_t i2 = it->Index2();

        Vector<double> v0 = coordinates[i0].GetVector();
        v0 += coordinates[i1].GetVector();
        v0 += coordinates[i2].GetVector();
        v0 /= 3.0;


        MeshRegion2dPtr mr(FindRegion(v0.Getx(), v0.Gety()));
        if (mr)
        {
            const std::string &nm = mr->GetName();
            if (!meshLoader->IsMeshRegion(nm))
            {
                const std::string &m = mr->GetMaterial();
                meshLoader->AddRegion(new MeshRegion(nm, m));

                std::ostringstream os;
                os << "Creating Region " << nm << "\n";
                OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str().c_str());
            }

            MeshRegion &mregion = meshLoader->GetMeshRegion(nm);
            std::vector<size_t> &ctonode = CoordinateToNode[nm];

            size_t &mni = MaxNodeIndex[nm];

            if (ctonode[i0] == size_t(-1))
            {
                ctonode[i0] = mni;
                mregion.AddNode(MeshNode(i0));
                ++nodesAtCoordinate[i0];
                ++mni;
            }
            if (ctonode[i1] == size_t(-1))
            {
                ctonode[i1] = mni;
                mregion.AddNode(MeshNode(i1));
                ++nodesAtCoordinate[i1];
                ++mni;
            }
            if (ctonode[i2] == size_t(-1))
            {
                ctonode[i2] = mni;
                mregion.AddNode(MeshNode(i2));
                ++nodesAtCoordinate[i2];
                ++mni;
            }

            size_t ni0 = ctonode[i0];
            size_t ni1 = ctonode[i1];
            size_t ni2 = ctonode[i2];

            mregion.AddEdge(MeshEdge(ni0, ni1));
            mregion.AddEdge(MeshEdge(ni1, ni2));
            mregion.AddEdge(MeshEdge(ni2, ni0));
            mregion.AddTriangle(MeshTriangle(ni0, ni1, ni2));
        }
        else
        {
#if 1
        std::ostringstream os;
        os << "Triangle has no region: "
            << coordinates[i0].GetVector() << " "
            << coordinates[i1].GetVector() << " "
            << coordinates[i2].GetVector() << " "
            << v0 << "\n";
        OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str().c_str());
#endif
        }
    }

    for (size_t i = 0; i < nodesAtCoordinate.size(); ++i)
    {
        size_t count = nodesAtCoordinate[i];
        //// TODO: some type of message for multiple interfaces
        if (count > 1)
        {
//TODO: Check Bounding box
            bool found = false;
            for (std::vector<std::string>::iterator ci = contactOrder.begin(); ci != contactOrder.end(); ++ci)
            {
                const std::string &nm = *ci;
                MeshContact2d &mc = *contacts[nm];

                if (!mc.IsPointInside(coordinates[i].GetX(), coordinates[i].GetY()))
                {
                    continue;
                }

                const std::string &r  = mc.GetRegion();
                const std::string &m  = mc.GetMaterial();

                if (!meshLoader->IsMeshContact(nm))
                {
                    meshLoader->AddContact(new MeshContact(nm, r, m));
                }

                CoordinateToNode_t::iterator cn = CoordinateToNode.find(r);
                if (cn != CoordinateToNode.end())
                {
                    size_t ni = (cn->second)[i];
                    if (ni != size_t(-1))
                    {
                        meshLoader->GetMeshContact(nm).AddNode(MeshNode(ni));
                        found = true;
            std::ostringstream os;
            os << "Contact: " << nm << " region " << r << " ni: " << ni << " ci: " << i << " " << coordinates[i].GetVector() << "\n";
            OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str().c_str());
                        break;
                    }
                }
                else
                {
//TODO: implement error here
                }
            }

            /// Now search interfaces
            if (!found)
            {
                for (std::vector<std::string>::iterator ii = interfaceOrder.begin(); ii != interfaceOrder.end(); ++ii)
                {
//TODO: Check Bounding box
                    const std::string &nm = *ii;
                    MeshInterface2d &mi = *interfaces[nm];

                    if (!mi.IsPointInside(coordinates[i].GetX(), coordinates[i].GetY()))
                    {
                        continue;
                    }

                    const std::string &r0  = mi.GetRegion0();
                    const std::string &r1  = mi.GetRegion1();

                    if (!meshLoader->IsMeshInterface(nm))
                    {
                        meshLoader->AddInterface(new MeshInterface(nm, r0, r1));
                    }

                    CoordinateToNode_t::iterator in0 = CoordinateToNode.find(r0);
                    CoordinateToNode_t::iterator in1 = CoordinateToNode.find(r1);
                    if ((in0 != CoordinateToNode.end()) && (in1 != CoordinateToNode.end()))
                    {
                        size_t ni0 = (in0->second)[i];
                        size_t ni1 = (in1->second)[i];
                        if ((ni0 != size_t(-1)) && (ni1 != size_t(-1)))
                        {
                            meshLoader->GetMeshInterface(nm).AddNodePair(MeshInterfaceNodePair(ni0, ni1));
                            found = true;
                            break;
                        }
                    }
                    else
                    {
//TODO: implement error here
                    }
                }
            }
        }
    }

    if (ret)
    {
        ret = meshLoader->Finalize(errorString);
    }

    if (ret)
    {
        this->SetFinalized();
    }

    return ret;
}

void Mesh2d::AddLine(LineDirection d, MeshLine2dPtr mline)
{
    if (d == dsMesh::LineDirection::XDIR)
    {
        xlines.push_back(mline);
    }
    else if (d == dsMesh::LineDirection::YDIR)
    {
        ylines.push_back(mline);
    }
}

void Mesh2d::AddRegion(MeshRegion2dPtr mreg)
{
    const std::string &nm = mreg->GetName();

    MeshRegion2dList_t::iterator it = regions.find(nm);
    if (it != regions.end())
    {
        MeshRegion2d &morig = *(it->second);
        if (morig.GetMaterial() != mreg->GetMaterial())
        {
            std::ostringstream os;
            os << "Not changing material from " << morig.GetMaterial() <<
                " to " << mreg->GetMaterial() << " for region " << nm << "\n";
            OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str().c_str());
        }
        morig.AddBoundingBox(*mreg);
    }
    else
    {
        regions[nm] = mreg;
        regionOrder.push_back(nm);
    }
}

void Mesh2d::AddInterface(MeshInterface2dPtr mint)
{
    const std::string &nm = mint->GetName();

    MeshInterface2dList_t::iterator it = interfaces.find(nm);
    if (it != interfaces.end())
    {
        MeshInterface2d &morig = *(it->second);
        if (
            (morig.GetRegion0() != mint->GetRegion0()) ||
            (morig.GetRegion1() != mint->GetRegion1())
        )
        {
            std::ostringstream os;
            os << "Not changing interface regions  for interface " << nm << "\n";
            OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str().c_str());
        }
        morig.AddBoundingBox(*mint);
    }
    else
    {
        interfaces[nm] = mint;
        interfaceOrder.push_back(nm);
    }
}

void Mesh2d::AddContact(MeshContact2dPtr mcon)
{
    const std::string &nm = mcon->GetName();

    MeshContact2dList_t::iterator it = contacts.find(nm);
    if (it != contacts.end())
    {
        MeshContact2d &morig = *(it->second);
        if (
            (morig.GetRegion() != mcon->GetRegion())
            )
        {
            std::ostringstream os;
            os << "Not changing contact region  for contact " << nm << "\n";
            OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str().c_str());
        }
        morig.AddBoundingBox(*mcon);
    }
    else
    {
        contacts[nm] = mcon;
        contactOrder.push_back(nm);
    }
}
}
