%{
/***
DEVSIM
Copyright 2016 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "Gmshyystype.hh"
#include "GmshReader.hh"
#include "GmshParser.hh"
#include <sstream>
#define YY_NO_UNPUT
#ifdef _WIN32
#include <io.h>
#define YY_NO_UNISTD_H
#endif
// remove clang compiler warning
#define register
%}
%option noyywrap
%option nounput
%option stack

integer [-]?[0-9]+
float1 [+-]?[0-9]*\.[0-9]+
float2 [+-]?[0-9]+\.[0-9]*
float3 [+-]?[0-9]+\.?[0-9]*[eE][+-]?[0-9]+

/* Unicode goodness from http://osdir.com/ml/lex.flex.general/2004-03/msg00023.html */
U80    [\200-\277]
U81    [\300-\337]{U80}
U82    [\340-\357]{U80}{2}
U83    [\360-\367]{U80}{3}
U84    [\370-\373]{U80}{4}
U85    [\374-\375]{U80}{5}
UTF8   {U81}|{U82}|{U83}|{U84}|{U85}

%x MESHFORMAT_SEC PHYSICALNAMES_SEC NODES_SEC ELEMENTS_SEC
%%
<*>[ \t\r]+	    ;

<*>[\n]	{++dsGmshParse::meshlineno; return EOL;}

<NODES_SEC>{float1}|{float2}|{float3}    {Gmshlval.dval = atof(yytext); return FLOAT;}

<PHYSICALNAMES_SEC,NODES_SEC,ELEMENTS_SEC>{integer}   {Gmshlval.dval = atof(yytext); Gmshlval.ival = atof(yytext); return INT;}

<INITIAL>^\$MeshFormat {
		yy_push_state(MESHFORMAT_SEC);
		return BEG_MESHFORMAT;
	    }

<MESHFORMAT_SEC>^\$EndMeshFormat {
		yy_pop_state();
		return END_MESHFORMAT;
	    }

<MESHFORMAT_SEC>^[0-9. ]+ {
		Gmshlval.str = yytext;
                return MESHFORMAT;
            }

<INITIAL>^\$PhysicalNames {
		yy_push_state(PHYSICALNAMES_SEC);
		return BEG_PHYSICALNAMES;
	    }

<PHYSICALNAMES_SEC>^\$EndPhysicalNames {
		yy_pop_state();
		return END_PHYSICALNAMES;
	    }

<INITIAL>^\$Nodes {
		yy_push_state(NODES_SEC);
		return BEG_NODES;
	    }

<NODES_SEC>^\$EndNodes {
		yy_pop_state();
		return END_NODES;
	    }


<INITIAL>^\$Elements {
		yy_push_state(ELEMENTS_SEC);
		return BEG_ELEMENTS;
	    }

<ELEMENTS_SEC>^\$EndElements {
		yy_pop_state();
		return END_ELEMENTS;
	    }

<PHYSICALNAMES_SEC>\"([A-Za-z]|{UTF8})([A-Za-z_0-9:@]|{UTF8})*\" {
                std::string temp(yytext);
                //// Get rid of the quotation marks
                temp = temp.substr(1, temp.size()-2);
		Gmshlval.str = temp;
		return WORD;
            }

<PHYSICALNAMES_SEC>([A-Za-z]|{UTF8})([A-Za-z_0-9:@]|{UTF8})*  {
		Gmshlval.str = yytext;
		return WORD;
	    }

<*>.   {return yytext[0];}
%%

int Gmshparse();
#include "GmshLoader.hh"
#include "MeshKeeper.hh"
namespace dsGmshParse {
bool LoadMeshesFromFile(const std::string &fname, const std::string &meshName, std::string &errorString)
{
    bool ret = false;
    dsGmshParse::errors.clear();
    dsGmshParse::meshlineno = 0;


    if (dsGmshParse::GmshLoader)
    {
      std::ostringstream os;
      os << "A Gmsh mesh is already being loaded by the name of " << dsGmshParse::GmshLoader->GetName() << "\n";
      errorString += os.str();
      return false;
    }
    else
    {
      dsGmshParse::GmshLoader = new dsMesh::GmshLoader(meshName);
      dsMesh::MeshKeeper &mk = dsMesh::MeshKeeper::GetInstance();
      mk.AddMesh(dsGmshParse::GmshLoader);
    }

    Gmshin = fopen(fname.c_str(), "r");

    int retval = 1;

    if ( !Gmshin )
    {
	std::ostringstream os;
	os << "Could not open file " << fname << "\n";
	errorString += os.str();
    }
    else
    {
        Gmsh_switch_to_buffer(Gmsh_create_buffer( Gmshin, YY_BUF_SIZE ) );
	retval = Gmshparse();
	yy_delete_buffer( YY_CURRENT_BUFFER );
    }


    ret = !retval;

    dsGmshParse::DeletePointers();
    errorString += dsGmshParse::errors;
    dsGmshParse::GmshLoader = NULL;
    return ret;
}
}

