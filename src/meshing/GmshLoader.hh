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

#ifndef GMSH_LOADER_HH
#define GMSH_LOADER_HH
#include "Mesh.hh"
#include "MeshLoaderStructs.hh"
#include <vector>
#include <map>
#include <string>
#include <utility>

class Coordinate;
class Node;
class Edge;
class Triangle;
class Tetrahedron;

namespace dsMesh {

class GmshLoader;
typedef GmshLoader *GmshLoaderPtr;

struct GmshElement {
  enum ElementType_t {UNKNOWN = 0, POINT, LINE, TRIANGLE, TETRAHEDRON};
  GmshElement(size_t ei, size_t pn, ElementType_t et, const std::vector<int> &ni) : element_index(ei), physical_number(pn), element_type(et), node_indexes(ni) {};

  size_t           element_index;
  size_t           physical_number;
  ElementType_t    element_type;
  std::vector<int> node_indexes;
};

struct GmshShapes {
  typedef std::vector<int>             NodeIndexes_t;
  typedef std::vector<MeshNode>        MeshNodeList_t;
  typedef std::vector<MeshEdge>        MeshEdgeList_t;
  typedef std::vector<MeshTriangle>    MeshTriangleList_t;
  typedef std::vector<MeshTetrahedron> MeshTetrahedronList_t;

  void AddShape(GmshElement::ElementType_t element_type, const NodeIndexes_t &node_indexes);

  void DecomposeAndUniquify();

  size_t GetDimension() const;

  size_t GetNumberOfTypes() const;

  MeshNodeList_t        Points;
  MeshEdgeList_t        Lines;
  MeshTriangleList_t    Triangles;
  MeshTetrahedronList_t Tetrahedra;
};

struct GmshInterfaceInfo {
  GmshInterfaceInfo(const std::string &iname, const std::string &rname0, const std::string &rname1) : interface(iname), region0(rname0), region1(rname1) {}
  GmshInterfaceInfo() {}

  const std::vector<std::string> &GetPhysicalNames() const
  {
    return physical_names;
  }

  void AddPhysicalName(const std::string &pname) {
    physical_names.push_back(pname);
  }

  std::vector <std::string> physical_names;
  std::string interface;
  std::string region0;
  std::string region1;
};

struct GmshContactInfo {
  GmshContactInfo(const std::string &cname, const std::string &rname) : contact(cname), region(rname) {}
  GmshContactInfo() {}

  const std::vector<std::string> &GetPhysicalNames() const
  {
    return physical_names;
  }

  void AddPhysicalName(const std::string &pname) {
    physical_names.push_back(pname);
  }

  std::string contact;
  std::string region;
  std::vector <std::string> physical_names;
};


struct GmshRegionInfo {
  GmshRegionInfo(const std::string &rname, const std::string &mname) : region(rname), material(mname) {}
  GmshRegionInfo() {}

  const std::vector<std::string> &GetPhysicalNames() const
  {
    return physical_names;
  }

  void AddPhysicalName(const std::string &pname) {
    physical_names.push_back(pname);
  }

  std::string               region;
  std::string               material;
  std::vector <std::string> physical_names;
};

class GmshLoader : public Mesh {
    public:

        typedef std::vector<std::pair<size_t, MeshCoordinate> >      MeshCoordinateList_t;
#if 0
        typedef std::map<std::string, MeshRegionPtr>     MeshRegionList_t;
        typedef std::map<std::string, MeshInterfacePtr>  MeshInterfaceList_t;
        typedef std::map<std::string, MeshContactPtr>    MeshContactList_t;
#endif
        typedef std::map<size_t, std::string>            PhysicalIndexToName_t;
        typedef std::map<std::string, GmshRegionInfo>    MapToRegionInfo_t;
        typedef std::map<std::string, GmshContactInfo>   MapToContactInfo_t;
        typedef std::map<std::string, GmshInterfaceInfo> MapToInterfaceInfo_t;
//        typedef std::map<std::string, std::string>      PhysicalNameToName_t;
//        typedef std::map<std::string, std::pair<std::string, std::string> > PhysicalNameToNamePair_t;
        typedef std::vector<GmshElement>                GmshElementList_t;
        typedef std::map<std::string, GmshShapes>       GmshShapesMap_t;             


        GmshLoader(const std::string &);
        ~GmshLoader();

        /// Could be called as an additional tclapi command or from file itself
        void AddPhysicalName(const size_t i, const std::string &nm)
        {
          //// TODO: ensure that the ID is unique and hasn't already been used
          physicalNameMap[i] = nm;
        }


        //// creates an implicit entry with null string
        const std::string &GetPhysicalName(const size_t i) const
        {
          return physicalNameMap[i];
        }

        bool HasPhysicalName(const size_t i) const;


        void AddCoordinate(size_t i, const MeshCoordinate &mc)
        {
          //// don't worry if a coordinate gets replaces
          meshCoordinateList.push_back(std::make_pair(i, mc));
          if (i > maxCoordinateIndex)
          {
            maxCoordinateIndex = i;
          }
        }

        //// First element of index is the physical group number
        void AddElement(const GmshElement &ge)
        {
          elementList.push_back(ge);
        }

        void MapPhysicalNameToContact(const std::string &pname, const std::string &cname, const std::string &rname)
        {
          ///rname is just in case our contact is interfacing two regions
          if (!pname.empty())
          {
            contactMap[cname].physical_names.push_back(pname);
          }
          if (!rname.empty())
          {
            contactMap[cname].region = rname;
          }
        }

        void MapPhysicalNameToInterface(const std::string &pname, const std::string &iname, const std::string &rname0, const std::string &rname1)
        {
          if (!pname.empty())
          {
            interfaceMap[iname].physical_names.push_back(pname);
          }
          if (!rname0.empty())
          {
            interfaceMap[iname].region0 = rname0;
          }
          if (!rname1.empty())
          {
            interfaceMap[iname].region1 = rname1;
          }
        }

        void MapPhysicalNameToRegion(const std::string &pname, const std::string &rname, const std::string &mname)
        {
          if (!pname.empty())
          {
            regionMap[rname].physical_names.push_back(pname);
          }
          if (!mname.empty())
          {
            regionMap[rname].material = mname;
          }
        }

    protected:
    private:
        bool Instantiate_(const std::string &, std::string &);
        bool Finalize_(std::string &);

        void GetUniqueNodesFromPhysicalNames(const std::vector<std::string> &, GmshShapes::MeshNodeList_t &);
        void GetUniqueTetrahedraFromPhysicalNames(const std::vector<std::string> &, GmshShapes::MeshTetrahedronList_t &);
        void GetUniqueTrianglesFromPhysicalNames(const std::vector<std::string> &, GmshShapes::MeshTriangleList_t &);
        void GetUniqueEdgesFromPhysicalNames(const std::vector<std::string> &, GmshShapes::MeshEdgeList_t &);


        void processNodes(const GmshShapes::MeshNodeList_t &, const std::vector<Coordinate *> &, std::vector<Node *> &);
        void processEdges(const GmshShapes::MeshEdgeList_t &, const std::vector<Node *> &, std::vector<const Edge *> &);
        void processTriangles(const GmshShapes::MeshTriangleList_t &, const std::vector<Node *> &, std::vector<const Triangle *> &);
        void processTetrahedra(const GmshShapes::MeshTetrahedronList_t &, const std::vector<Node *> &, std::vector<const Tetrahedron *> &);

        GmshLoader();
        GmshLoader(const GmshLoader &);
        GmshLoader &operator=(const GmshLoader &);

        mutable PhysicalIndexToName_t physicalNameMap;
        MeshCoordinateList_t          meshCoordinateList;
        GmshElementList_t             elementList;
        MapToContactInfo_t            contactMap;
        MapToInterfaceInfo_t          interfaceMap;
        MapToRegionInfo_t             regionMap;

        GmshShapesMap_t               gmshShapesMap;             
        size_t                        dimension;
        size_t                        maxCoordinateIndex;
};

}
#endif

