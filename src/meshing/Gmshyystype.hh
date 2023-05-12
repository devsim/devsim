/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef GMSH_YYSTYPE_HH
#define GMSH_YYSTYPE_HH
#include <string>
#include <vector>
typedef struct {
    std::string str;
    double           dval;
    int              ival;
    std::vector<int> ilist;
} gmshyystype;
#define YYSTYPE gmshyystype
#endif

