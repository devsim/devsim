/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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
