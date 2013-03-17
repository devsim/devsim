/***
DEVSIM
Copyright 2013 Devsim LLC

This file is part of DEVSIM.

DEVSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, version 3.

DEVSIM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with DEVSIM.  If not, see <http://www.gnu.org/licenses/>.
***/

#ifndef TEST_HH
#define TEST_HH

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

int adifferror(const char *);
int adifflex();
int adiffparse();
void adiffrestart(FILE *);
extern FILE * adiffin;
void prompt();
// flex stuff to parse string
#define yyconst const
typedef struct yy_buffer_state *YY_BUFFER_STATE;
YY_BUFFER_STATE adiff_scan_string( yyconst char *yy_str );
void adiff_delete_buffer( YY_BUFFER_STATE b );

std::string CreateErrorMessage(const Eqo::EqObjPtr &, const std::string &, const Eqo::EqObjPtr &, const std::string &);
#endif
