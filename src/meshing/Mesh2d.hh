/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef MESH2D_HH
#define MESH2D_HH
#include "Mesh.hh"
#include "Mesh2dStructs.hh"
#include "MeshLoaderStructs.hh"


#include <memory>
#include <vector>
#include <string>
#include <set>
#include <map>

namespace dsMesh {
enum class LineDirection {XDIR = 0, YDIR};
class Mesh2d;
typedef Mesh2d *Mesh2dPtr;

class DevsimLoader;

class MeshLine2d;
typedef std::shared_ptr<MeshLine2d> MeshLine2dPtr;
class MeshRegion2d;
typedef std::shared_ptr<MeshRegion2d> MeshRegion2dPtr;
class MeshInterface2d;
typedef std::shared_ptr<MeshInterface2d> MeshInterface2dPtr;
class MeshContact2d;
typedef std::shared_ptr<MeshContact2d> MeshContact2dPtr;

class Mesh2d : public Mesh {
    public:
        typedef std::vector<MeshLine2dPtr> MeshLineVector_t;

        ~Mesh2d();
        /// Should we throw an exception if there is a problem?
        /// Such as adding a point to a finalized mesh
/*
        //// This is so users can add arbitrary points before triangulations
        void   AddMeshPoint(const MeshPoint &);
*/
        void   AddLine(LineDirection, MeshLine2dPtr);
        void   AddRegion(MeshRegion2dPtr);
        void   AddContact(MeshContact2dPtr);
        void   AddInterface(MeshInterface2dPtr);

/*
        const std::vector<double> &GetLocations();
*/

        Mesh2d(const std::string &);

    private:
        std::vector<double> GetLocations(MeshLineVector_t &);
        bool Instantiate_(const std::string &, std::string &);
        bool Finalize_(std::string &);

        MeshRegion2dPtr FindRegion(double /*x*/, double /*y*/);

        Mesh2d &operator=(const Mesh2d &);
        Mesh2d(const Mesh2d &);

        /// begin by requiring points in order
        std::string                     name;
        MeshLineVector_t                xlines;
        MeshLineVector_t                ylines;
        typedef std::map<std::string, MeshRegion2dPtr>  MeshRegion2dList_t;
        typedef std::map<std::string, MeshContact2dPtr> MeshContact2dList_t;
        typedef std::map<std::string, MeshInterface2dPtr> MeshInterface2dList_t;
        MeshRegion2dList_t    regions;
        MeshContact2dList_t   contacts;
        MeshInterface2dList_t interfaces;
        std::vector<std::string>        regionOrder;
        std::vector<std::string>        interfaceOrder;
        std::vector<std::string>        contactOrder;

        std::shared_ptr<DevsimLoader> meshLoader;

};

inline bool operator<(MeshLine2dPtr p0, MeshLine2dPtr p1)
{
    return p0->getPosition() < p1->getPosition();
}

}

#endif
