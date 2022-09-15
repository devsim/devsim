/***
DEVSIM
Copyright 2013 Devsim LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
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
  std::vector<std::string> vararg;   // variable number of arguments
  std::vector<Eqo::EqObjPtr> varobj; // variable number of objects
} myyystype;

#define YYSTYPE myyystype

int adifferror(const char *);
int adifflex();
int adiffparse();
void adiffrestart(FILE *);
extern FILE *adiffin;
void prompt();
// flex stuff to parse string
#define yyconst const
typedef struct yy_buffer_state *YY_BUFFER_STATE;
YY_BUFFER_STATE adiff_scan_string(yyconst char *yy_str);
void adiff_delete_buffer(YY_BUFFER_STATE b);

std::string CreateErrorMessage(const Eqo::EqObjPtr &, const std::string &,
                               const Eqo::EqObjPtr &, const std::string &);
#endif
