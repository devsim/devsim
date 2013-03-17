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

#ifndef GMSH_READER_HH
#define GMSH_READER_HH
#include <string>
#include <cstddef>


//TODO: ensure stack size can trigger realloc

namespace dsMesh {
class GmshLoader;
typedef GmshLoader   *GmshLoaderPtr;

class MeshRegion;
typedef MeshRegion   *MeshRegionPtr;

class MeshContact;
typedef MeshContact   *MeshContactPtr;

class MeshInterface;
typedef MeshInterface   *MeshInterfacePtr;

class Solution;
typedef Solution   *SolutionPtr;
}

namespace dsGmshParse {
extern int meshlineno;
extern dsMesh::GmshLoaderPtr    GmshLoader;
//extern dsMesh::MeshRegionPtr    MeshRegion;
//extern dsMesh::MeshContactPtr   MeshContact;
//extern dsMesh::MeshInterfacePtr MeshInterface;
//extern dsMesh::SolutionPtr      Sol;
extern std::string errors;

void DeletePointers();

bool LoadMeshes(const std::string &/*filename*/, const std::string&/*meshName*/, std::string &/*errorString*/);
}

#endif


