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
    MeshPtr mp = nullptr;
    if (meshList.count(nm))
    {
        mp = meshList[nm];
    }
    return mp;
}
}

