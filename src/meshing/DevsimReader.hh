/***
DEVSIM
Copyright 2013 DEVSIM LLC

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


