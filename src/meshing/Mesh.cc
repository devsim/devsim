/***
DEVSIM
Copyright 2013 DEVSIM LLC

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
