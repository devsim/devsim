/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "GmshReader.hh"
#include "GmshLoader.hh"
#include "MeshKeeper.hh"
#include <sstream>

namespace dsGmshParse {
int meshlineno;
dsMesh::GmshLoaderPtr    GmshLoader = nullptr;
//dsMesh::MeshRegionPtr    MeshRegion = nullptr;
//dsMesh::MeshContactPtr   MeshContact = nullptr;
//dsMesh::MeshInterfacePtr MeshInterface = nullptr;
//dsMesh::SolutionPtr      Sol= nullptr;
std::string errors;
}

namespace dsGmshParse
{
void DeletePointers()
{
}

bool LoadMeshesFromArgs(const std::string &meshName, const std::vector<double> &coordinates, const std::vector<std::string> &physical_names, const std::vector<size_t> &elements, std::string &errorString)
{
  dsMesh::GmshLoaderPtr gmshLoaderp = new dsMesh::GmshLoader(meshName);
  dsMesh::MeshKeeper &mk = dsMesh::MeshKeeper::GetInstance();
  mk.AddMesh(gmshLoaderp);

  dsMesh::GmshLoader &gmshLoader = *gmshLoaderp;

  size_t num_coords = coordinates.size()/3;
  if (3*num_coords != coordinates.size())
  {
    errorString += "The coordinate list length must be divisible by 3\n";
    return false;
  }

  for (size_t i = 0; i < num_coords; ++i)
  {
   gmshLoader.AddCoordinate(i, dsMesh::MeshCoordinate(coordinates[3*i], coordinates[3*i + 1], coordinates[3*i+2]));
  }

  size_t element_vec_len = elements.size();
  size_t element_count = 0;
  size_t eeindex = 0;
  size_t nodes_to_get = 0;
  size_t pdim = 0;
  std::vector<int> node_indexes;
  std::map<size_t, size_t> physical_dimensions;
  for (eeindex = 0; eeindex < element_vec_len;)
  {
    dsMesh::Shapes::ElementType_t element_type = static_cast<dsMesh::Shapes::ElementType_t>(elements[eeindex]);
    switch (element_type)
    {
      case dsMesh::Shapes::ElementType_t::POINT:
        nodes_to_get = 1;
        pdim = 0;
        break;
      case dsMesh::Shapes::ElementType_t::LINE:
        nodes_to_get = 2;
        pdim = 1;
        break;
      case dsMesh::Shapes::ElementType_t::TRIANGLE:
        nodes_to_get = 3;
        pdim = 2;
        break;
      case dsMesh::Shapes::ElementType_t::TETRAHEDRON:
        nodes_to_get = 4;
        pdim = 3;
        break;
      default:
        nodes_to_get = 0;
        pdim = size_t(-1);
        break;
    }

    if (nodes_to_get == 0)
    {
      std::ostringstream os;
      os << "ERROR: element " << static_cast<size_t>(element_type) << " unrecognized type position " << eeindex << "\n";
      errorString = os.str();
      return false;
    }
    else if ((eeindex + nodes_to_get + 1) > element_vec_len)
    {
      std::ostringstream os;
      os << "ERROR: element list indexing error on element " << element_count << "\n";
      errorString = os.str();
      return false;
    }

    size_t physical_number = elements[eeindex + 1];

    if (physical_number > physical_names.size())
    {
      std::ostringstream os;
      os << "ERROR: element physical index " << physical_number << " must be less than number of physical_names " << physical_names.size() <<  " on element " << element_count << " on position " << eeindex + 1 << "\n";
      errorString = os.str();
      return false;
    }
    else if (physical_dimensions.count(physical_number) == 0)
    {
      physical_dimensions[physical_number] = pdim;
    }
    else if (physical_dimensions[physical_number] != pdim)
    {
      std::ostringstream os;
      os << "ERROR: element dimension " << pdim << " must match other elements " << physical_dimensions[physical_number] <<  " on element " << element_count << " on position " << eeindex + 1 << "\n";
      errorString = os.str();
      return false;
    }

    node_indexes.resize(nodes_to_get);

    for (size_t i = 0; i < nodes_to_get; ++i)
    {
      node_indexes[i] = elements[eeindex + i + 2];
    }

    gmshLoader.AddElement(dsMesh::GmshElement(element_count, physical_number, element_type, node_indexes));

    ++element_count;
    eeindex += nodes_to_get + 2;
  }

  if (eeindex != element_vec_len)
  {
    std::ostringstream os;
    os << "ERROR: mismatch in element indexing at position " << eeindex << "\n";
    errorString = os.str();
    return false;
  }

  for (size_t i = 0; i < physical_names.size(); ++i)
  {
    const std::string &name = physical_names[i];
    std::map<size_t, size_t>::iterator it = physical_dimensions.find(i);
    if (it != physical_dimensions.end())
    {
      gmshLoader.AddPhysicalName(it->second, i, name);
    }
    else
    {
      std::ostringstream os;
      os << "ERROR: physical name \"" << name << "\" index " << i << " has no elements assigned\n";
      errorString = os.str();
      return false;
    }
  }

  return true;
}
}

