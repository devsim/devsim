/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef GMSH_LOADER_HH
#define GMSH_LOADER_HH
#include "Mesh.hh"
#include "MeshLoaderStructs.hh"
#include "MeshLoaderUtility.hh"
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
  GmshElement(size_t ei, size_t pn, Shapes::ElementType_t et, const std::vector<int> &ni) : element_index(ei), physical_number(pn), element_type(et), node_indexes(ni) {
    /* this is an optimization as we now currently allow only one element type per dimension */
    dimension=static_cast<size_t>(element_type);
  };

  size_t           element_index;
  size_t           physical_number;
  size_t           dimension;
  Shapes::ElementType_t element_type;
  std::vector<int> node_indexes;
};


struct GmshInterfaceInfo {
  GmshInterfaceInfo(const std::string &iname, const std::string &rname0, const std::string &rname1) : interface(iname), region0(rname0), region1(rname1) {}
  GmshInterfaceInfo() {}

  std::vector <std::string> physical_names;
  std::string interface;
  std::string region0;
  std::string region1;
};

struct GmshContactInfo {
  GmshContactInfo(const std::string &cname, const std::string &rname, const std::string &mname) : contact(cname), region(rname), material(mname) {}
  GmshContactInfo() {}

  std::vector <std::string> physical_names;
  std::string contact;
  std::string region;
  std::string material;
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
        typedef std::map<size_t, std::string>            PhysicalIndexToName_t;
        typedef std::vector<PhysicalIndexToName_t>       PhysicalDimensionIndexToName_t;
        typedef std::map<std::string, GmshRegionInfo>    MapToRegionInfo_t;
        typedef std::map<std::string, GmshContactInfo>   MapToContactInfo_t;
        typedef std::map<std::string, GmshInterfaceInfo> MapToInterfaceInfo_t;
        typedef std::vector<GmshElement>                 GmshElementList_t;
        typedef std::map<std::string, Shapes>            ShapesMap_t;


        GmshLoader(const std::string &);
        ~GmshLoader();

        void AddPhysicalName(const size_t d, const size_t i, const std::string &nm)
        {
          physicalDimensionIndexNameMap[d][i] = nm;
        }

        //// creates an implicit entry with null string
        const std::string &GetPhysicalName(const size_t d, const size_t i) const
        {
          return physicalDimensionIndexNameMap[d][i];
        }

        bool HasPhysicalName(const size_t d, const size_t i) const;


        void AddCoordinate(size_t i, const MeshCoordinate &mc)
        {
          //// don't worry if a coordinate gets replaced
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

        void MapPhysicalNameToContact(const std::string &pname, const std::string &cname, const std::string &rname, const std::string &mname)
        {
          ///rname is just in case our contact is interfacing two regions
          if (!pname.empty())
          {
            contactMap[cname].physical_names.push_back(pname);
          }
          if (!rname.empty())
          {
            contactMap[cname].region = rname;
            contactMap[cname].material = mname;
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

        void GetUniqueNodesFromPhysicalNames(const std::vector<std::string> &, MeshNodeList_t &);
        void GetUniqueTetrahedraFromPhysicalNames(const std::vector<std::string> &, MeshTetrahedronList_t &);
        void GetUniqueTrianglesFromPhysicalNames(const std::vector<std::string> &, MeshTriangleList_t &);
        void GetUniqueEdgesFromPhysicalNames(const std::vector<std::string> &, MeshEdgeList_t &);


        GmshLoader();
        GmshLoader(const GmshLoader &);
        GmshLoader &operator=(const GmshLoader &);

        mutable PhysicalDimensionIndexToName_t physicalDimensionIndexNameMap;
        MeshCoordinateList_t          meshCoordinateList;
        GmshElementList_t             elementList;
        MapToContactInfo_t            contactMap;
        MapToInterfaceInfo_t          interfaceMap;
        MapToRegionInfo_t             regionMap;

        ShapesMap_t                   gmshShapesMap;
        size_t                        dimension;
        size_t                        maxCoordinateIndex;
};
}
#endif

