/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "Mesh1d.hh"
#include "Mesh1dStructs.hh"
#include "MeshUtil.hh"

#include "GlobalData.hh"
#include "Device.hh"
#include "Region.hh"
#include "Coordinate.hh"
#include "Node.hh"
#include "Edge.hh"
#include "Contact.hh"
#include "Interface.hh"
#include "ModelCreate.hh"

#include "OutputStream.hh"

#include "dsAssert.hh"

#include <algorithm>
#include <vector>
#include <sstream>

namespace dsMesh {
Mesh1d::Mesh1d(const std::string &n) : Mesh(n)
{
}

void Mesh1d::AddLine(const MeshLine1d &pt)
{
    points.push_back(pt);
}

void Mesh1d::AddRegion(const MeshRegion1d &r)
{
    regions.push_back(r);
}

void Mesh1d::AddInterface(const MeshInterface1d &i)
{
    interfaces.push_back(i);
}

void Mesh1d::AddContact(const MeshContact1d &c)
{
    contacts.push_back(c);
}

void Mesh1d::SetLocationsAndTags()
{
    dsAssert(!points.empty(), "UNEXPECTED");

    sort(points.begin(), points.end());

    double xl = points[0].getPosition();
    double sl = points[0].getPositiveSpacing();

    std::string tag = points[0].GetTag();

    locations.push_back(xl);
    if (!tag.empty())
    {
        dsAssert(!tagsToIndices.count(tag), "UNEXPECTED");
        dsAssert(!indicesToTags.count(0), "UNEXPECTED");
        /// We are at the first position in the list
        tagsToIndices[tag] = 0;
        indicesToTags[0]   = tag;
    }

    double xh;
    double sh;
    for (size_t i = 1; i < points.size(); ++i)
    {
        xh  = points[i].getPosition();
        sh  = points[i].getNegativeSpacing();
        tag = points[i].GetTag();

        /// Get points for this section
        MeshUtil::pts_t pts = MeshUtil::getPoints(xl, xh, sl, sh);
        for (size_t j = 1; j < pts.size(); ++j)
        {
            locations.push_back(pts[j]);
        }

        if (!tag.empty())
        {
            size_t pos = locations.size() - 1;

            std::ostringstream os;
            os << tag << "\n";
            OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());

            dsAssert(!tagsToIndices.count(tag), "UNEXPECTED");
            dsAssert(!indicesToTags.count(pos), "UNEXPECTED");
            /// We are at the first position in the list
            tagsToIndices[tag] = pos;
            indicesToTags[pos] = tag;
        }

        xl = xh;
        sl = points[i].getPositiveSpacing();
    }
}


namespace {
//// This functor is to sort regions by their lowest index
struct RegionSort
{
    bool operator()(const MeshRegion1d &r0, const MeshRegion1d &r1) const
    {
        dsAssert(r0.GetIndex0() < r0.GetIndex1(), "UNEXPECTED");
        dsAssert(r1.GetIndex0() < r1.GetIndex1(), "UNEXPECTED");
        return (r0.GetIndex0() < r1.GetIndex1());
    }
};
}

bool Mesh1d::SanityCheckingAndSetInterfaceIndices(std::string &errorString)
{
    bool ret = true;
    std::ostringstream os;

    dsAssert(indexToInterfaceIndex.empty(), "UNEXPECTED");

    size_t endindex = locations.size() - 1;

    /// Contact checking
    do
    {
        //// Assume that we must have two contacts
        if (contacts.size() != 2)
        {
            ret = false;
            os << "must have 2 contacts for a 1d mesh.  One at each end.\n";
            break;
        }

        dsAssert(tagsToIndices.count(contacts[0].GetTag()) != 0, "UNEXPECTED");
        dsAssert(tagsToIndices.count(contacts[1].GetTag()) != 0, "UNEXPECTED");
        size_t pos0 = tagsToIndices[contacts[0].GetTag()];
        size_t pos1 = tagsToIndices[contacts[1].GetTag()];

        if (pos0 == pos1)
        {
            ret = false;
            os << "Contacts can't be at the same point\n";
            break;
        }

        if (!(((pos0 == 0) && (pos1 == endindex)) ||
              ((pos1 == 0) && (pos0 == endindex))))
        {
            ret = false;
            os << "Contacts must be at both ends of 1d device\n";
            break;
        }

        indexToContactIndex[pos0] = 0;
        indexToContactIndex[pos1] = 1;
    } while (0);

    if (!ret)
    {
        errorString += os.str();
        return ret;
    }


    //// tag index to interface index
    std::set<std::string>    interfaceNames;
    for (size_t i = 0; i < interfaces.size(); ++i)
    {
        const std::string &interfaceTag  = interfaces[i].GetTag();
        const std::string &interfaceName = interfaces[i].GetName();

        if (interfaceNames.count(interfaceName))
        {
            ret = false;
            os << "Interface name " << interfaceName << " can only be used once in a mesh\n";
            break;
        }
        else
        {
            interfaceNames.insert(interfaceName);
        }

        if (!tagsToIndices.count(interfaceTag))
        {
            ret = false;
            os << "Tag " << interfaceTag << " specified for interface " << interfaces[i].GetName() << " does not exist\n";
            break;
        }

        size_t tagIndex = tagsToIndices[interfaceTag];

        if (indexToInterfaceIndex.count(tagIndex))
        {
            ret = false;
            os << "No more than 1 interface can be specified at tag " << interfaceTag << "\n";
            break;
        }
        else
        {
            indexToInterfaceIndex[tagIndex] = i;
        }
    }
    if (!ret)
    {
        errorString += os.str();
        return ret;
    }

    //// Assume that every point in the mesh has to be colored with a region
    size_t cpos = 0;
    for (size_t i = 0; i < regions.size(); ++i)
    {
        size_t i0 = regions[i].GetIndex0();
        size_t i1 = regions[i].GetIndex1();

        indexToRegionIndex[i0] = i;

        dsAssert(i0 < i1, "UNEXPECTED");

        if (cpos != i0)
        {
            ret = false;
            os << "Discontinuity in 1d meshing at region " << regions[i].GetName() << "\n";
            break;
        }
        else
        {
            cpos = i1;
            if (cpos != endindex)
            {
                if (!indexToInterfaceIndex.count(cpos))
                {
                    ret = false;
                    os << "Interface does not exist at tag " << indicesToTags[cpos] << "\n";
                    break;
                }
            }
        }
    }
    if (!ret)
    {
        errorString += os.str();
        return ret;
    }

    if ((cpos != endindex) && !regions.empty())
    {
        ret = false;
        os << "Discontinuity in 1d meshing at region " << regions.back().GetName() << "\n";
        errorString += os.str();
        return ret;
    }

    if (indexToInterfaceIndex.count(0) || indexToInterfaceIndex.count(endindex))
    {
        ret = false;
        os << "Can't specify interface at end of device\n";
        errorString += os.str();
        return ret;
    }

    if (regions.size() == 0)
    {
        ret = false;
        os << "must specify at least one region for meshing\n";
        errorString += os.str();
        return ret;
    }

    for (size_t i = 0; i < locations.size(); ++i)
    {
        if (indexToInterfaceIndex.count(i))
        {
            if (!indexToRegionIndex.count(i))
            {
                ret = false;
                os << "Error, can't have interface without Region start " << interfaces[indexToInterfaceIndex[i]].GetName() << "\n";
                break;
            }
        }
    }
    //// Assume that a region can't be split by another interface

    //// Must specify interface wherever two regions meet

    //// Handle an arbitrary number of interfaces anywhere except the ends

    errorString += os.str();
    return ret;
}

bool Mesh1d::Finalize_(std::string &errorString)
{
    bool ret = true;

    std::ostringstream os;

    if (points.size() < 2)
    {
        ret = false;
        os << this->GetName() << "Must have at least 2 points to be finalized\n";
        errorString += os.str();
        return ret;
    }

    SetLocationsAndTags();

    //// Here we enforce that the first index must be less than the second
    for (size_t i = 0; i < regions.size(); ++i)
    {
        regions[i].sortTags(tagsToIndices);
    }
    //// Here we sort the list from min region position to top region position
    sort(regions.begin(), regions.end(), RegionSort());

    ret = SanityCheckingAndSetInterfaceIndices(errorString);
    if (!ret)
    {
        return ret;
    }

    size_t j = 0;
    for (size_t i = 0; i < locations.size(); ++i)
    {
        std::ostringstream os;
//      os << locations[i];

        if (j < regions.size())
        {
            size_t i0 = regions[j].GetIndex0();
            if (i0 == i)
            {
                os << " (region: " << regions[j].GetName() << ")\n";
                ++j;
            }
        }

        if (indexToInterfaceIndex.count(i))
        {
            os << " (interface: " << interfaces[indexToInterfaceIndex[i]].GetName() << ")\n";
        }

        if (indexToContactIndex.count(i))
        {
            os << " (contact: " << contacts[indexToContactIndex[i]].GetName() << ")\n";
        }
//      os << "\n";

        OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
    }

    if (ret)
    {
        this->SetFinalized();
    }

    errorString += os.str();
    return ret;
}

Mesh1d::~Mesh1d() {
}

bool Mesh1d::Instantiate_(const std::string &DeviceName, std::string &errorString)
{
    bool ret = true;

    GlobalData &GInst = GlobalData::GetInstance();
    const size_t dimension = 1;

    Device *dev = new Device(DeviceName, dimension);
    GInst.AddDevice(dev);

    std::vector<CoordinatePtr> coord_list;
    for (size_t i = 0; i < locations.size(); ++i)
    {
        CoordinatePtr cp = new Coordinate(locations[i], 0.0, 0.0);
        coord_list.push_back(cp);
    }
    dev->AddCoordinateList(coord_list);

    std::vector<std::pair<Node *, Node *> > node_pairs;
    std::vector<Region *> region_pointers;
    for (size_t i = 0; i < regions.size(); ++i)
    {
        MeshRegion1d &r = regions[i];
        RegionPtr rp = new Region(r.GetName(), r.GetMaterial(), dimension, dev);
        region_pointers.push_back(rp);

        size_t i0 = r.GetIndex0();
        size_t i1 = r.GetIndex1();
        std::vector<NodePtr> node_list;
//      std::vector<EdgePtr> edge_list;
        for (size_t j = i0; j <= i1; ++j)
        {
            size_t ni = j - i0;
            NodePtr np = new Node(ni, coord_list[j]);
            node_list.push_back(np);


            rp->AddNode(np);

            if (ni > 0)
            {
                EdgePtr ep = new Edge(ni, node_list[ni-1], node_list[ni]);
//              edge_list.push_back(ep);
                rp->AddEdge(ep);
            }
        }
        node_pairs.push_back(std::make_pair(node_list.front(), node_list.back()));

        rp->FinalizeMesh();

        dev->AddRegion(rp);

        CreateDefaultModels(rp);
    }

    //// Contacts
    {
        for (size_t i = 0; i < contacts.size(); ++i)
        {
            Node   *np;
            Region *rp;

            dsAssert(tagsToIndices.count(contacts[i].GetTag()) != 0, "UNEXPECTED");

            size_t index = tagsToIndices[contacts[i].GetTag()];
            if (index == 0)
            {
                np = node_pairs[0].first;
                rp = region_pointers[0];
            }
            else
            {
                //// Then it must be at the very last location
                np = node_pairs.back().second;
                rp = region_pointers.back();
            }
            ConstNodeList_t cnp;
            cnp.push_back(np);
            Contact *ctop = new Contact(contacts[i].GetName(), rp, cnp, contacts[i].GetMaterial()); // pass dev pointer
            dev->AddContact(ctop); // make implicit part of contact constructor
        }
    }
    //// Interfaces
    {
        for (size_t i = 1; i < regions.size(); ++i)
        {
            ConstNodeList i0;
            ConstNodeList i1;
            i0.push_back(node_pairs[i-1].second);
            i1.push_back(node_pairs[i].first);

            dsAssert(indexToInterfaceIndex.count(regions[i-1].GetIndex1()) != 0, "UNEXPECTED");
            size_t ind = indexToInterfaceIndex[regions[i-1].GetIndex1()];

            Interface *myint = new Interface(interfaces[ind].GetName(), region_pointers[i-1], region_pointers[i], i0, i1);
            dev->AddInterface(myint);
        }
    }

    return ret;
}

const std::vector<double> &Mesh1d::GetLocations() {
    dsAssert(IsFinalized(), "UNEXPECTED");
    return locations;
}
}
