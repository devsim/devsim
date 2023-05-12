/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "InterfaceNodeScalarData.hh"
#include "InterfaceNodeModel.hh"
#include "ScalarData.cc"

#define SCCLASSNAME InterfaceNodeScalarData
#define SCMODELTYPE InterfaceNodeModel
#define SCDBLTYPE   double
#include "ScalarDataInstantiate.cc"

#ifdef DEVSIM_EXTENDED_PRECISION
#undef  SCDBLTYPE
#define SCDBLTYPE float128
#include "Float128.hh"
#include "ScalarDataInstantiate.cc"
#endif

