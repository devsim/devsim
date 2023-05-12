/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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
    delete instance;
    instance = nullptr;
}


MeshKeeper::~MeshKeeper()
{
    MeshList_t::iterator mit = meshList.begin();
    for ( ; mit != meshList.end(); ++mit)
    {
        delete mit->second;
    }
    meshList.clear();
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

bool MeshKeeper::DeleteMesh(const std::string &nm)
{
    bool ret = false;
    if (auto it = meshList.find(nm); it != meshList.end())
    {
        auto mp = it->second;
        meshList.erase(it);
        delete mp;
        ret = true;
    }
    return ret;
}
}

