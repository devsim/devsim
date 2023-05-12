/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef DEVSIM_YYSTYPE_HH
#define DEVSIM_YYSTYPE_HH
#include <string>
typedef struct {
    std::string str;
    double      dval;
    int         ival;
} devsimyystype;
#define YYSTYPE devsimyystype
#endif

