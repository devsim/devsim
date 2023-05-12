/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef TETRAHEDRON_EDGE_SCALAR_DATA_HH
#define TETRAHEDRON_EDGE_SCALAR_DATA_HH
#include <vector>

template<typename T>
using TetrahedronEdgeScalarList = std::vector<T>;

#include "ScalarData.hh"

class TetrahedronEdgeModel;

template<typename T>
using TetrahedronEdgeScalarData = ScalarData<TetrahedronEdgeModel, T>;

class EdgeModel;

template<typename T>
using EdgeScalarData = ScalarData<EdgeModel, T>;

#endif

