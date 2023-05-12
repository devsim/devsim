/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef DEVSIM_READER_HH
#define DEVSIM_READER_HH
#include <string>
#include <cstddef>

//TODO: ensure stack size can trigger realloc

namespace dsMesh {
class DevsimLoader;
typedef DevsimLoader   *DevsimLoaderPtr;

class MeshRegion;
typedef MeshRegion   *MeshRegionPtr;

class MeshContact;
typedef MeshContact   *MeshContactPtr;

class MeshInterface;
typedef MeshInterface   *MeshInterfacePtr;

class Solution;
typedef Solution   *SolutionPtr;

class Equation;
typedef Equation   *EquationPtr;
}

namespace dsDevsimParse {
extern int meshlineno;
extern dsMesh::DevsimLoaderPtr    DevsimLoader;
extern dsMesh::MeshRegionPtr    MeshRegion;
extern dsMesh::MeshContactPtr   MeshContact;
extern dsMesh::MeshInterfacePtr MeshInterface;
extern dsMesh::SolutionPtr      Sol;
extern dsMesh::EquationPtr      Equation;
extern std::string errors;

void DeletePointers();

bool LoadMeshes(const std::string &/*filename*/, std::string &/*errorString*/);
}

#endif


