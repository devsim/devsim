/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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
