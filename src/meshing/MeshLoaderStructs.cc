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

#include "MeshLoaderStructs.hh"

namespace dsMesh {
namespace {
#if 0
template <typename T> void DeletePointersFromVector(T &x)
{
    typename T::iterator it = x.begin();
    for ( ; it != x.end(); ++it)
    {
        delete *it;
    }
}
#endif

template <typename T> void DeletePointersFromMap(T &x)
{
    typename T::iterator it = x.begin();
    for ( ; it != x.end(); ++it)
    {
        delete it->second;
    }
}
}

const char * Solution::ModelTypeString[] = {
    "MUNDEFINED",
    "Node",
    "Edge",
    "Triangle Edge",
    "Tetrahedron Edge",
    "Interface Node"};

MeshContact::~MeshContact()
{
//    DeletePointersFromMap<SolutionList_t>(solutionList);
    DeletePointersFromMap<EquationList_t>(equationList);
}


MeshInterface::~MeshInterface()
{
    DeletePointersFromMap<SolutionList_t>(solutionList);
    DeletePointersFromMap<EquationList_t>(equationList);
}

MeshRegion::~MeshRegion()
{
    DeletePointersFromMap<SolutionList_t>(solutionList);
    DeletePointersFromMap<EquationList_t>(equationList);
}
}

