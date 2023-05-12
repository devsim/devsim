/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef TRIANGLE_EDGE_SCALAR_DATA_HH
#define TRIANGLE_EDGE_SCALAR_DATA_HH
#include <vector>

template<typename T>
using TriangleEdgeScalarList = std::vector<T>;

#include "ScalarData.hh"

class TriangleEdgeModel;

template<typename T>
using TriangleEdgeScalarData = ScalarData<TriangleEdgeModel, T>;

class EdgeModel;

template<typename T>
using EdgeScalarData = ScalarData<EdgeModel, T>;

#endif

