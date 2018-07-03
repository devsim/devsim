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

#include "Mesh1dStructs.hh"
#include "dsAssert.hh"

namespace dsMesh {
MeshLine1d::MeshLine1d(double p, double ps, double ns) :
    position(p), posSpacing(ps), negSpacing(ns)
{
}

MeshLine1d::MeshLine1d(double p, double ps, double ns, const std::string &t) :
    position(p), posSpacing(ps), negSpacing(ns), tag(t)
{
}

MeshRegion1d::MeshRegion1d(const std::string &r, const std::string &m, const std::string &t1, const std::string &t2)
: name(r), material(m), tag0(t1), tag1(t2), tagssorted(false)
{
}
const std::string &MeshRegion1d::GetName() const
{
    return name;
}

const std::string &MeshRegion1d::GetMaterial() const
{
    return material;
}

const std::string &MeshRegion1d::GetTag0() const
{
    dsAssert(tagssorted, "UNEXPECTED");
    return tag0;
}

const std::string &MeshRegion1d::GetTag1() const
{
    dsAssert(tagssorted, "UNEXPECTED");
    return tag1;
}

const size_t &MeshRegion1d::GetIndex0() const
{
    dsAssert(tagssorted, "UNEXPECTED");
    return index0;
}

const size_t &MeshRegion1d::GetIndex1() const
{
    dsAssert(tagssorted, "UNEXPECTED");
    return index1;
}

void MeshRegion1d::sortTags(const std::map<std::string, size_t> &v)
{
    dsAssert(v.count(tag0) != 0, std::string("line for tag ") + tag0 + " does not exist");
    dsAssert(v.count(tag1) != 0, std::string("line for tag ") + tag1 + " does not exist");

    index0 = v.find(tag0)->second;
    index1 = v.find(tag1)->second;

    dsAssert(index0 != index1, "UNEXPECTED");

    if (index1 < index0)
    {
        size_t t = index0;
        index0   = index1;
        index1   = t;
    }
    tagssorted = true;
}

MeshInterface1d::MeshInterface1d(const std::string &n, const std::string &t) : name(n), tag(t)
{
}

const std::string &MeshInterface1d::GetName()
{
    return name;
}

const std::string &MeshInterface1d::GetTag()
{
    return tag;
}

MeshContact1d::MeshContact1d(const std::string &n, const std::string &m, const std::string &t) : name(n), material(m), tag(t)
{
}

const std::string &MeshContact1d::GetMaterial()
{
    return material;
}

const std::string &MeshContact1d::GetName()
{
    return name;
}

const std::string &MeshContact1d::GetTag()
{
    return tag;
}
}
