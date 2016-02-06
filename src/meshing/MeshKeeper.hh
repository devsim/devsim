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
