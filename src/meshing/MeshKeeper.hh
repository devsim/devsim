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
