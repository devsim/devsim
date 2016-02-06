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

#ifndef GENIUS_LOADER_HH
#define GENIUS_LOADER_HH
#include "Mesh.hh"
#include "MeshLoaderStructs.hh"
#include "MeshLoaderUtility.hh"
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <utility>

class Coordinate;
class Node;
class Edge;
class Triangle;
class Tetrahedron;
class ObjectHolder;

namespace dsMesh {

class GeniusLoader;
typedef GeniusLoader *GeniusLoaderPtr;

struct GeniusRegion;
typedef std::shared_ptr<GeniusRegion> GeniusRegionPtr;
typedef std::weak_ptr<GeniusRegion> GeniusRegionWeakPtr;
struct GeniusBoundary;
typedef std::shared_ptr<GeniusBoundary> GeniusBoundaryPtr;

struct GeniusInterfaceInfo {
  GeniusInterfaceInfo(const std::string &iname, const std::string &rname0, const std::string &rname1) : interface(iname), region0(rname0), region1(rname1) {}
  GeniusInterfaceInfo() {}

  std::string boundary_name;
  std::string interface;
  std::string region0;
  std::string region1;
};

struct GeniusContactInfo {
  GeniusContactInfo(const std::string &cname, const std::string &rname, const std::string &mname) : contact(cname), region(rname), material(mname) {}
  GeniusContactInfo() {}

  std::string boundary_name;
  std::string contact;
  std::string region;
  std::string material;
};

struct GeniusBoundary {
  GeniusBoundary() : electrode_potential(0.0), electrode_potential_old(0.0), electrode_vapp(0.0), electrode_iapp(0.0),
has_electrode(false) {}
  std::string name;
  std::string region_name;
  std::string label;
  std::string settings;
  std::vector<int> points;
  double electrode_potential;
  double electrode_potential_old;
  double electrode_vapp;
  double electrode_iapp;
  GeniusRegionWeakPtr region_ptr;
  bool has_electrode;
};

struct GeniusRegionInfo {
  GeniusRegionInfo(const std::string &rname, const std::string &mname) : region(rname), material(mname) {}
  GeniusRegionInfo() {}

  void AddGeniusRegionName(const std::string &rname) {
    genius_region_name = rname;
  }

  std::string region;
  std::string material;
  std::string genius_region_name;
};

struct GeniusRegion {
  GeniusRegion(const std::string &rname, const std::string &mname) : region(rname), material(mname) {}
  GeniusRegion() {}
  typedef std::vector<Shapes> ShapesList_t;
  typedef std::vector<double> Solution_t;
  typedef std::map<std::string, Solution> SolutionMap_t;

  void AddShape(Shapes::ElementType_t element_type, const int *node_indexes)
  {
    genius_shapes.AddShape(element_type, node_indexes);
  }

  size_t GetDimension() const
  {
    return genius_shapes.GetDimension();
  }

  size_t GetNumberOfTypes() const
  {
    return genius_shapes.GetNumberOfTypes();
  }

  bool HasBoundary(const std::string &name) const;

  GeniusBoundary &GetBoundary(const std::string &name);

  std::string               region;
  std::string               material;
  std::map<std::string, std::vector<double> > solutions;
  std::map<std::string, std::string> units;
  std::vector<GeniusBoundaryPtr> bocos;
  Shapes              genius_shapes;
  std::vector<double>       vec_x;
  std::vector<double>       vec_y;
  std::vector<double>       vec_z;
  std::vector<int>          global_node_index;
  std::vector<int>          edata;
  int                       nelem;
};


class GeniusLoader : public Mesh {
    public:

        typedef std::vector<MeshCoordinate>            MeshCoordinateList_t;
        typedef std::map<std::string, GeniusRegionPtr> MapToGeniusRegion_t;
        typedef std::map<std::string, std::vector<GeniusBoundaryPtr> >    MapToGeniusBoundary_t;

        typedef std::map<std::string, GeniusRegionInfo>    MapToRegionInfo_t;
        typedef std::map<std::string, GeniusContactInfo>   MapToContactInfo_t;
        typedef std::map<std::string, GeniusInterfaceInfo> MapToInterfaceInfo_t;
        typedef std::map<std::string, Shapes>        ShapesMap_t;             


        GeniusLoader(const std::string &);
        ~GeniusLoader();


        void SetDimension(size_t x)
        {
          dimension = x;
        }

        size_t GetDimension() const
        {
          return dimension;
        }

        void SetResistive(bool x)
        {
          resistive_metal = x;
        }

        bool GetResistive()
        {
          return resistive_metal;
        }

        void SetMagicNumber(const std::string &x)
        {
          magic_number = x;
        }

        const std::string &GetMagicNumber()
        {
          return magic_number;
        }

        void AddRegion(GeniusRegionPtr);

        bool HasGeniusRegion(const std::string &name)
        {
          return geniusRegionMap.count(name);
        }

        GeniusRegion &GetGeniusRegion(const std::string &name)
        {
          return *(geniusRegionMap[name]);
        }

        bool HasRegionInfo(const std::string &name)
        {
          return regionMap.count(name);
        }

        GeniusRegionInfo &GetRegionInfo(const std::string &name)
        {
          return regionMap[name];
        }

        ObjectHolder GetMeshInfo();

        void MapNameToContact(const std::string &bname, const std::string &cname, const std::string &rname, const std::string &mname)
        {
          ///rname is just in case our contact is interfacing two regions
          if (!bname.empty())
          {
            contactMap[cname].boundary_name = bname;
          }
          if (!rname.empty())
          {
            contactMap[cname].region = rname;
            contactMap[cname].material = mname;
          }
        }

        void MapNameToInterface(const std::string &bname, const std::string &iname, const std::string &rname0, const std::string &rname1)
        {
          if (!bname.empty())
          {
            interfaceMap[iname].boundary_name = bname;
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

        void MapNameToRegion(const std::string &bname, const std::string &rname, const std::string &mname)
        {
          if (!bname.empty())
          {
            regionMap[rname].genius_region_name = bname;
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

        void AddCoordinate(size_t i, const MeshCoordinate &mc)
        {
          if (i < meshCoordinateList.size())
          {
            meshCoordinateList[i] = mc;
          }
          else
          {
            meshCoordinateList.resize(i+1);
            meshCoordinateList[i] = mc;
          }
        }

        std::vector<int> *GetGeniusRegionBoundaryPoints(const std::string &genius_region_name, const std::string &genius_boundary_name);

        GeniusLoader();
        GeniusLoader(const GeniusLoader &);
        GeniusLoader &operator=(const GeniusLoader &);

        MeshCoordinateList_t          meshCoordinateList;
        MapToGeniusRegion_t           geniusRegionMap;
        MapToGeniusBoundary_t         geniusBoundaryMap;
        MapToRegionInfo_t             regionMap;
        MapToContactInfo_t            contactMap;
        MapToInterfaceInfo_t          interfaceMap;
        ShapesMap_t                   geniusShapesMap;             
        std::string                   magic_number;
        size_t                        dimension;
        bool                          resistive_metal;
};

}
#endif

