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

#ifndef VTK_WRITER_HH
#define VTK_WRITER_HH
#include "MeshWriter.hh"
#include <string>
/// Start out by writing the all out to one file
class VTKWriter : public MeshWriter {
public:
  ~VTKWriter();

private:
  bool WriteMeshes_(const std::string & /*filename*/,
                    std::string & /*errorString*/);
  bool WriteMesh_(const std::string & /*deviceName*/,
                  const std::string & /*filename*/,
                  std::string & /*errorString*/);
};
#endif
