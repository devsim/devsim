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

#ifndef MESH_HH
#define MESH_HH
#include <string>
namespace dsMesh {
class Mesh {
    public:
        virtual ~Mesh() = 0;
        const std::string &GetName();

        bool Instantiate(const std::string &, std::string &);
        bool Finalize(std::string &);
        bool IsFinalized();

    protected:
        virtual bool Instantiate_(const std::string &, std::string &) = 0;
        virtual bool Finalize_(std::string &) = 0;

        explicit Mesh(const std::string &);
        void     SetFinalized();
    private:
        Mesh();
        Mesh(const Mesh &);
        Mesh &operator=(const Mesh&);

        std::string name;
        bool finalized;
};

}
#endif
