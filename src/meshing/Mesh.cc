/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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
