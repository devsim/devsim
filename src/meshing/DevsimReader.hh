/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef DEVSIM_READER_HH
#define DEVSIM_READER_HH
#include "MeshLoaderStructs.hh"
#include <string>
#include <cstddef>

//TODO: ensure stack size can trigger realloc

namespace dsMesh {
class DevsimLoader;
typedef DevsimLoader   *DevsimLoaderPtr;
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


