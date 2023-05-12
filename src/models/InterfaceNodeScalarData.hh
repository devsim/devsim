/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef INTERFACE_NODE_SCALAR_DATA_HH
#define INTERFACE_NODE_SCALAR_DATA_HH
#include <vector>
template<typename T>
using NodeScalarList = std::vector<T>;

class InterfaceNodeModel;

#include "ScalarData.hh"
template<typename T>
using InterfaceNodeScalarData = ScalarData<InterfaceNodeModel, T>;

#endif

