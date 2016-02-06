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
//    DeletePointersFromMap<MeshSolutionList_t>(solutionList);
    DeletePointersFromMap<MeshEquationList_t>(equationList);
}


MeshInterface::~MeshInterface()
{
    DeletePointersFromMap<MeshSolutionList_t>(solutionList);
    DeletePointersFromMap<MeshEquationList_t>(equationList);
}

MeshRegion::~MeshRegion()
{
    DeletePointersFromMap<MeshSolutionList_t>(solutionList);
    DeletePointersFromMap<MeshEquationList_t>(equationList);
}
}

