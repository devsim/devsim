/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "MeshWriter.hh"

MeshWriter::~MeshWriter() {}

bool MeshWriter::WriteMesh(const std::string &deviceName, const std::string &filename, std::vector<std::string> &include, std::vector<std::string> &exclude, std::string &errorString)
{
    return this->WriteMesh_(deviceName, filename, include, exclude, errorString);
}

bool MeshWriter::WriteMeshes(const std::string &filename, std::vector<std::string> &include, std::vector<std::string> &exclude, std::string &errorString)
{
    return this->WriteMeshes_(filename, include, exclude, errorString);
}

