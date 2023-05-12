/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef NODE_SCALAR_DATA_HH
#define NODE_SCALAR_DATA_HH
#include <vector>

template<typename T>
using NodeScalarList = std::vector<T>;

#include "ScalarData.hh"
class NodeModel;
template<typename T>
using NodeScalarData = ScalarData<NodeModel, T>;

#endif
