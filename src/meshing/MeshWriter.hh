/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef MESH_WRITER_HH
#define MESH_WRITER_HH
#include "ObjectHolder.hh"
#include <string>
#include <functional>
typedef std::function<bool(const std::string &)> MeshWriterTest_t;
class MeshWriter {
 public:
  /// Start out by writing the all out to one file
  virtual ~MeshWriter() = 0;
  bool WriteMeshes(const std::string & /*filename*/, ObjectHolder /*include*/,
                   std::string & /*errorString*/);
  bool WriteMesh(const std::string & /*deviceName*/,
                 const std::string & /*filename*/, ObjectHolder /*include*/,
                 std::string & /*errorString*/);

 private:
  virtual bool WriteMeshes_(const std::string & /*filename*/,
                            MeshWriterTest_t /*include*/,
                            std::string & /*errorString*/) = 0;
  virtual bool WriteMesh_(const std::string & /*deviceName*/,
                          const std::string & /*filename*/,
                          MeshWriterTest_t /*include*/,
                          std::string & /*errorString*/) = 0;
};
#endif
