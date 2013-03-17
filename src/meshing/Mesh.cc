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

#include "Mesh.hh"
#include <sstream>
namespace dsMesh {
Mesh::Mesh(const std::string &n) : name(n), finalized(false)
{
}

Mesh::~Mesh()
{
}

const std::string &Mesh::GetName()
{
    return name;
}

bool Mesh::IsFinalized()
{
    return finalized;
}

void Mesh::SetFinalized()
{
    finalized = true;
}

bool Mesh::Instantiate(const std::string &name, std::string &errorString)
{
    bool ret = true;
    if (!IsFinalized())
    {
        ret = false;
        std::ostringstream os;
        os << this->GetName() << " has not been finalized\n";
        errorString += os.str();
        return ret;
    }

    ret = this->Instantiate_(name, errorString);
    return ret;
}

bool Mesh::Finalize(std::string &errorString)
{
    bool ret = true;
    if (IsFinalized())
    {
        ret = false;
        std::ostringstream os;
        os << this->GetName() << " has already been finalized\n";
        errorString += os.str();
        return ret;
    }

    ret = this->Finalize_(errorString);
    return ret;
}
}
