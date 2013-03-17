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

