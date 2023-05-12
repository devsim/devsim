/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "TriangleEdgeScalarData.hh"
#include "TriangleEdgeModel.hh"
#include "ScalarData.cc"

#define SCCLASSNAME TriangleEdgeScalarData
#define SCMODELTYPE TriangleEdgeModel
#define SCDBLTYPE   double
#include "ScalarDataInstantiate.cc"

#ifdef DEVSIM_EXTENDED_PRECISION
#undef  SCDBLTYPE
#define SCDBLTYPE float128
#include "Float128.hh"
#include "ScalarDataInstantiate.cc"
#endif

