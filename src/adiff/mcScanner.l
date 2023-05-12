%{
/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/


// must include test.hh first
// to use struct instead of union.
#include <cstdlib>
#include "mcTest.hh"
#include "mcParser.hh"
#include "mcModelCompiler.hh"
#include "EquationObject.hh"
#include "UserFunc.hh"
#define yylval mclval
#define YY_NO_UNPUT
// For isatty
#ifdef _WIN32
#include <io.h>
#define YY_NO_UNISTD_H
#else
#include <unistd.h>
#endif
// remove clang compiler warning
#define register
%}
%option noyywrap
%option nounput
/*%option stack*/

integer [0-9]+
float1 [0-9]*\.[0-9]+
float2 [0-9]+\.[0-9]*
float3 [0-9]+\.?[0-9]*[eE][+-]?[0-9]+

U80    [\200-\277]
U81    [\300-\337]{U80}
U82    [\340-\357]{U80}{2}
U83    [\360-\367]{U80}{3}
U84    [\370-\373]{U80}{4}
U85    [\374-\375]{U80}{5}
UTF8   {U81}|{U82}|{U83}|{U84}|{U85}


%%
[ \t\r\n]*	    ;

#.*$		    ;

{float1}|{float2}|{float3}    {yylval.dval = atof(yytext); return FLOAT;}

{integer}   {yylval.dval = atoi(yytext); return INT;}

^title  {yylval.str=yytext; return TITLE;}

^help	{yylval.str=yytext; return HELP;}

^model {yylval.str=yytext; return MOD_CMD;}

^equation {yylval.str=yytext; return EQ_CMD;}

^external_node {yylval.str=yytext; return EXTN_CMD;}

^internal_node {yylval.str=yytext; return INTN_CMD;}

^parameter {yylval.str=yytext; return PARAM_CMD;}

^declarefunc {yylval.str=yytext; return DECUSERFUNC;}

^definefuncdiff {yylval.str=yytext; return DEFUSERFUNC;}

^link {yylval.str=yytext; return LINK_CMD;}

ddt {yylval.str=yytext; return DDT;}

\"[^"]*\" {yylval.str=yytext; return STRING;}

\<         {yylval.str=yytext; return LT;}
\<=        {yylval.str=yytext; return LTE;}
\>         {yylval.str=yytext; return GT;}
\>=        {yylval.str=yytext; return GTE;}
==         {yylval.str=yytext; return EQ;}
!=         {yylval.str=yytext; return NEQ;}
!          {yylval.str=yytext; return NOT;}
&&         {yylval.str=yytext; return AND;}
\|\|       {yylval.str=yytext; return OR;}

[A-Za-z][A-Za-z_0-9:]*  {yylval.str = yytext;
		if (Eqo::getUnaryFuncPtr(yylval.str) != NULL)
		    return UNARYFUNC;
		else if (Eqo::getBinaryFuncPtr(yylval.str) != NULL)
		    return BINARYFUNC;
		else if (Eqo::UserFuncMap.count(yylval.str))
		    return USERFUNC;
	        else if (isInModelList(yylval.str))
		    return MODELNAME;
	        else if (EquationList.count(yylval.str) > 0)
		    return EQUATIONNAME;
	        else if (find(ExternalNodeList.begin(), ExternalNodeList.end(), yylval.str) != ExternalNodeList.end())
		    return EXTERNALNODE;
	        else if (find(InternalNodeList.begin(), InternalNodeList.end(), yylval.str) != InternalNodeList.end())
		    return INTERNALNODE;
	        else if (ParameterList.count(yylval.str) > 0)
		    return PARAMNAME;
		else
		    return VAR;  // VAR's may become anything else after processing
		    }

{UTF8}	{
	    mcerror("cannot currently generate source code with utf-8 parameter names.\n");
	    exit(-1);
	}

.   {return yytext[0];}
%%


