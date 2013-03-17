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

#include "DevsimReader.hh"
#include "DevsimLoader.hh"

namespace dsDevsimParse {
int meshlineno;
dsMesh::DevsimLoaderPtr  DevsimLoader = NULL;
dsMesh::MeshRegionPtr    MeshRegion = NULL;
dsMesh::MeshContactPtr   MeshContact = NULL;
dsMesh::MeshInterfacePtr MeshInterface = NULL;
dsMesh::SolutionPtr      Sol= NULL;
dsMesh::EquationPtr      Equation= NULL;
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
//    DevsimLoader = NULL;
    MeshRegion = NULL;
    MeshContact = NULL;
    MeshInterface = NULL;
    Sol = NULL;
    Equation = NULL;
}
}

