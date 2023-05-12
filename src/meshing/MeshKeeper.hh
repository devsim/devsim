/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef MESHKEEPER_HH
#define MESHKEEPER_HH
#include <string>
#include <map>

namespace dsMesh {
class Mesh;
typedef Mesh *MeshPtr;

// This class is supposed to contain everything
class MeshKeeper
{
    public:
        typedef std::map<std::string, MeshPtr> MeshList_t;

        static MeshKeeper &GetInstance();
        static void DestroyInstance();

        void AddMesh(MeshPtr);

        const MeshList_t &GetMeshList()
        {
            return meshList;
        }

        MeshPtr GetMesh(const std::string &);
        bool DeleteMesh(const std::string &);

    private:
        MeshKeeper();
        MeshKeeper(MeshKeeper &);
        MeshKeeper &operator=(MeshKeeper &);
        ~MeshKeeper();

        static MeshKeeper *instance;

        MeshList_t meshList;
};
}

#endif
