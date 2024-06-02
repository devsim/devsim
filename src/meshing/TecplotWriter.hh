/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef TECPLOT_WRITER_HH
#define TECPLOT_WRITER_HH
#include "MeshWriter.hh"
#include <string>
#include <vector>
/// Start out by writing the all out to one file
class TecplotWriter : public MeshWriter {
    public:
        ~TecplotWriter();
    private:
        bool WriteMeshes_(const std::string &/*filename*/, MeshWriterTest_t /*include*/, std::string &/*errorString*/);
        bool WriteMesh_(const std::string &/*deviceName*/, const std::string &/*filename*/, MeshWriterTest_t /*include*/, std::string &/*errorString*/);
};
#endif
