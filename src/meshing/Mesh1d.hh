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

#ifndef MESH1D_HH
#define MESH1D_HH
#include "Mesh.hh"
#include "Mesh1dStructs.hh"

#include <map>
#include <set>
#include <string>
#include <vector>

namespace dsMesh {
class Mesh1d;
typedef Mesh1d *Mesh1dPtr;

class Mesh1d : public Mesh {
public:
  ~Mesh1d();
  /// Should we throw an exception if there is a problem?
  /// Such as adding a point to a finalized mesh
  void AddLine(const MeshLine1d &);
  void AddRegion(const MeshRegion1d &);
  void AddContact(const MeshContact1d &);
  void AddInterface(const MeshInterface1d &);

  const std::vector<double> &GetLocations();

  Mesh1d(const std::string &);

private:
  bool Instantiate_(const std::string &, std::string &);
  bool Finalize_(std::string &);

  void SetLocationsAndTags();
  bool SanityCheckingAndSetInterfaceIndices(std::string &);

  Mesh1d &operator=(const Mesh1d &);
  Mesh1d(const Mesh1d &);

  /// begin by requiring points in order
  std::string name;
  std::vector<MeshLine1d> points;
  std::vector<double> locations;
  std::vector<MeshRegion1d> regions;
  std::vector<MeshContact1d> contacts;
  std::vector<MeshInterface1d> interfaces;

  std::map<size_t, size_t> indexToInterfaceIndex;
  std::map<size_t, size_t> indexToRegionIndex;
  std::map<size_t, size_t> indexToContactIndex;
  std::map<std::string, size_t> tagsToIndices;
  std::map<size_t, std::string> indicesToTags;
};
} // namespace dsMesh
#endif
