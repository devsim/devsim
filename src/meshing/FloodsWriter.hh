/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef FLOODS_WRITER_HH
#define FLOODS_WRITER_HH
#include "MeshWriter.hh"
#include <string>
/// Start out by writing the all out to one file
class FloodsWriter : public MeshWriter {
    public:
        ~FloodsWriter();
    private:
        bool WriteMeshes_(const std::string &/*filename*/, std::vector<std::string> &/*include*/, std::vector<std::string> &/*exclude*/, std::string &/*errorString*/);
        bool WriteMesh_(const std::string &/*deviceName*/, const std::string &/*filename*/, std::vector<std::string> &/*include*/, std::vector<std::string> &/*exclude*/, std::string &/*errorString*/);
};
#endif
