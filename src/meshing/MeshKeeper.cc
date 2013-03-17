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

#include "MeshKeeper.hh"
#include "Mesh.hh"

#include "dsAssert.hh"

namespace dsMesh {

MeshKeeper *MeshKeeper::instance = 0;

MeshKeeper::MeshKeeper()
{
}

MeshKeeper &MeshKeeper::GetInstance()
{
    if (!instance)
    {
        instance = new MeshKeeper;
    }
    return *instance;
}

void MeshKeeper::DestroyInstance()
{
    if (instance)
    {
        delete instance;
    }
    instance = 0;
}


MeshKeeper::~MeshKeeper()
{
    MeshList_t::iterator mit = meshList.begin();
    for ( ; mit != meshList.end(); ++mit)
    {
        delete mit->second;
    }
}

void MeshKeeper::AddMesh(MeshPtr mp)
{
    /// Instead of assertions, we should return a flag
    const std::string &nm = mp->GetName();
    dsAssert(meshList.count(nm) == 0, "UNEXPECTED");
    meshList[nm]=mp;
}

MeshPtr MeshKeeper::GetMesh(const std::string &nm)
{
    MeshPtr mp = NULL;
    if (meshList.count(nm))
    {
        mp = meshList[nm];
    }
    return mp;
}
}

