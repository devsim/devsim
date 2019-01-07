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

#include "DevsimReader.hh"
#include "DevsimLoader.hh"

namespace dsDevsimParse {
int meshlineno;
dsMesh::DevsimLoaderPtr  DevsimLoader = nullptr;
dsMesh::MeshRegionPtr    MeshRegion = nullptr;
dsMesh::MeshContactPtr   MeshContact = nullptr;
dsMesh::MeshInterfacePtr MeshInterface = nullptr;
dsMesh::SolutionPtr      Sol= nullptr;
dsMesh::EquationPtr      Equation= nullptr;
std::string errors;
}

namespace dsDevsimParse
{
void DeletePointers()
{
//    delete DevsimLoader;
    delete MeshRegion;
    delete MeshContact;
    delete MeshInterface;
    delete Sol;
    delete Equation;

    //// This is owned by the meshkeeper
    //// We should look to see if this is wise to keep around.  But, it has already been added to the MeshKeeper
//    DevsimLoader = nullptr;
    MeshRegion = nullptr;
    MeshContact = nullptr;
    MeshInterface = nullptr;
    Sol = nullptr;
    Equation = nullptr;
}
}

