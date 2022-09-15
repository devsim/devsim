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

#ifndef DEVSIM_LOADER_HH
#define DEVSIM_LOADER_HH
#include "Mesh.hh"
#include "MeshLoaderStructs.hh"
#include <map>
#include <string>
#include <vector>

namespace dsMesh {

class DevsimLoader : public Mesh {
public:
  typedef std::vector<MeshCoordinate> MeshCoordinateList_t;
  typedef std::map<std::string, MeshRegionPtr> MeshRegionList_t;
  typedef std::map<std::string, MeshInterfacePtr> MeshInterfaceList_t;
  typedef std::map<std::string, MeshContactPtr> MeshContactList_t;

  DevsimLoader(const std::string &);
  ~DevsimLoader();

  void AddCoordinate(const MeshCoordinate &cp) { coordinateList.push_back(cp); }

  void AddCoordinates(const std::vector<MeshCoordinate> &cp) {
    coordinateList = cp;
  }

  void AddRegion(MeshRegionPtr rp) { regionList[rp->GetName()] = rp; }

  void AddInterface(MeshInterfacePtr ip) { interfaceList[ip->GetName()] = ip; }

  void AddContact(MeshContactPtr cp) { contactList[cp->GetName()] = cp; }

  bool IsMeshRegion(const std::string &n) { return regionList.count(n) != 0; }

  MeshRegion &GetMeshRegion(const std::string &n) {
    return *(regionList.find(n)->second);
  }

  bool IsMeshContact(const std::string &n) { return contactList.count(n) != 0; }

  MeshContact &GetMeshContact(const std::string &n) {
    return *(contactList.find(n)->second);
  }

  bool IsMeshInterface(const std::string &n) {
    return interfaceList.count(n) != 0;
  }

  MeshInterface &GetMeshInterface(const std::string &n) {
    return *(interfaceList.find(n)->second);
  }

protected:
private:
  bool Instantiate_(const std::string &, std::string &);
  bool Finalize_(std::string &);

  DevsimLoader();
  DevsimLoader(const DevsimLoader &);
  DevsimLoader &operator=(const DevsimLoader &);

  MeshCoordinateList_t coordinateList;
  MeshRegionList_t regionList;
  MeshInterfaceList_t interfaceList;
  MeshContactList_t contactList;

  bool RunCommand(const std::string & /*commandString*/,
                  const std::vector<std::pair<std::string, ObjectHolder>> &,
                  std::string & /*resultString*/);
};

} // namespace dsMesh
#endif
