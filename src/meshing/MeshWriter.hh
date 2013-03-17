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

#ifndef MESH_WRITER_HH
#define MESH_WRITER_HH
#include <string>
class MeshWriter
{
    public:
        /// Start out by writing the all out to one file
        virtual ~MeshWriter() = 0;
        bool WriteMeshes(const std::string &/*filename*/, std::string &/*errorString*/);
        bool WriteMesh(const std::string &/*deviceName*/, const std::string &/*filename*/, std::string &/*errorString*/);
    private:
        virtual bool WriteMeshes_(const std::string &/*filename*/, std::string &/*errorString*/) = 0;
        virtual bool WriteMesh_(const std::string &/*deviceName*/, const std::string &/*filename*/, std::string &/*errorString*/) = 0;

};
#endif
