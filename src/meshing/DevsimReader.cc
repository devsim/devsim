/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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

