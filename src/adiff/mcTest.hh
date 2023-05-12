/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef MC_TEST_HH
#define MC_TEST_HH

#include <string>
#include <vector>

#include "EquationObject.hh"

typedef struct {
    std::string str;
    double dval;
    Eqo::EqObjPtr eqptr;
    std::vector<std::string> vararg; // variable number of arguments
    std::vector<Eqo::EqObjPtr> varobj; // variable number of objects
} myyystype;

#define YYSTYPE myyystype

int mcerror(const char *);
int yylex();
int mcparse();

// flex stuff to parse string
#define yyconst const
typedef struct yy_buffer_state *YY_BUFFER_STATE;
YY_BUFFER_STATE mc_scan_string( yyconst char *yy_str );
void mc_delete_buffer( YY_BUFFER_STATE b );

#endif
