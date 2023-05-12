/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "TetrahedronEdgeScalarData.hh"
#include "TetrahedronEdgeModel.hh"
#include "ScalarData.cc"

#define SCCLASSNAME TetrahedronEdgeScalarData
#define SCMODELTYPE TetrahedronEdgeModel
#define SCDBLTYPE   double
#include "ScalarDataInstantiate.cc"

#ifdef DEVSIM_EXTENDED_PRECISION
#undef  SCDBLTYPE
#define SCDBLTYPE float128
#include "Float128.hh"
#include "ScalarDataInstantiate.cc"
#endif

