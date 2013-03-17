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

#ifndef MESH1D_HH
#define MESH1D_HH
#include "Mesh.hh"
#include "Mesh1dStructs.hh"


#include <vector>
#include <string>
#include <set>
#include <map>

namespace dsMesh {
class Mesh1d;
typedef Mesh1d *Mesh1dPtr;

class Mesh1d : public Mesh {
    public:
        ~Mesh1d();
        /// Should we throw an exception if there is a problem?
        /// Such as adding a point to a finalized mesh
        void   AddLine(const MeshLine1d &);
        void   AddRegion(const MeshRegion1d &);
        void   AddContact(const MeshContact1d &);
        void   AddInterface(const MeshInterface1d &);

        const std::vector<double> &GetLocations();

        Mesh1d(const std::string &);

    private:
        bool Instantiate_(const std::string &, std::string &);
        bool Finalize_(std::string &);

        void SetLocationsAndTags();
        bool SanityCheckingAndSetInterfaceIndices(std::string &);

        Mesh1d &operator=(const Mesh1d &);
        Mesh1d(const Mesh1d &);

        /// begin by requiring points in order
        std::string                     name;
        std::vector<MeshLine1d>         points;
        std::vector<double>             locations;
        std::vector<MeshRegion1d>       regions;
        std::vector<MeshContact1d>      contacts;
        std::vector<MeshInterface1d>    interfaces;

        std::map<size_t, size_t>         indexToInterfaceIndex;
        std::map<size_t, size_t>         indexToRegionIndex;
        std::map<size_t, size_t>         indexToContactIndex;
        std::map<std::string, size_t>    tagsToIndices;
        std::map<size_t, std::string>    indicesToTags;
};
}
#endif
