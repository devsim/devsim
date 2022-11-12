/***
DEVSIM
Copyright 2013 DEVSIM LLC

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

#ifndef EDGE_SCALAR_DATA_HH
#define EDGE_SCALAR_DATA_HH
#include <vector>

template<typename T>
using EdgeScalarList = std::vector<T>;

template<typename T>
using NodeScalarList = std::vector<T>;

#include "ScalarData.hh"

class EdgeModel;

template<typename T>
using EdgeScalarData = ScalarData<EdgeModel, T>;

class Region;

template <typename DoubleType>
void createEdgeModelsFromNodeModel(const NodeScalarList<DoubleType> &, const Region &, EdgeScalarList<DoubleType> &, EdgeScalarList<DoubleType> &);

#endif
