/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef GMSH_READER_HH
#define GMSH_READER_HH
#include "GmshLoader.hh"
#include <memory>
#include <string>
#include <cstddef>
#include <vector>

namespace dsGmshParse {
extern int meshlineno;
extern dsMesh::GmshLoaderPtr    GmshLoader;
extern std::string errors;

void DeletePointers();

bool LoadMeshesFromFile(const std::string &/*filename*/, const std::string&/*meshName*/, std::string &/*errorString*/);
bool LoadMeshesFromArgs(const std::string &/*meshName*/, const std::vector<double> &/*coordinate_list*/, const std::vector<std::string> &/*physical_names*/, const std::vector<size_t> &/*element_list*/, std::string &/*errorString*/);

}

#endif


