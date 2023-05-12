/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef DS_GENERAL_ERRORS_HH
#define DS_GENERAL_ERRORS_HH
namespace dsErrors {
namespace ModelInfo {
enum ModelType {
  NODE = 0,
  EDGE,
  ELEMENTEDGE,
  INTERFACE
};

extern const char *ModelTypeStrings[];
}

}
#endif

