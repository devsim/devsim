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

#include "MeshWriter.hh"

MeshWriter::~MeshWriter() {}

bool MeshWriter::WriteMesh(const std::string &deviceName, const std::string &filename, std::string &errorString)
{
    return this->WriteMesh_(deviceName, filename, errorString);
}

bool MeshWriter::WriteMeshes(const std::string &filename, std::string &errorString)
{
    return this->WriteMeshes_(filename, errorString);
}

