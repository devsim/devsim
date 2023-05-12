/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "NodeScalarData.hh"
#include "NodeModel.hh"
#include "ScalarData.cc"

#define SCCLASSNAME NodeScalarData
#define SCMODELTYPE NodeModel
#define SCDBLTYPE   double
#include "ScalarDataInstantiate.cc"

#ifdef DEVSIM_EXTENDED_PRECISION
#undef  SCDBLTYPE
#define SCDBLTYPE float128
#include "Float128.hh"
#include "ScalarDataInstantiate.cc"
#endif

