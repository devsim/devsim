%{
/***
DEVSIM
Copyright 2016 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "Devsimyystype.hh"
#include "DevsimReader.hh"
#include "DevsimParser.hh"
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

%x DEVICE_SEC COORDINATE_SEC REGION_SEC NODE_SEC EDGE_SEC TRIANGLE_SEC TETRAHEDRON_SEC INTERFACE_SEC CONTACT_SEC NODESOL_SEC EDGESOL_SEC NODEMODEL_SEC EDGEMODEL_SEC TRIANGLEEDGEMODEL_SEC TETRAHEDRONEDGEMODEL_SEC INTERFACENODEMODEL_SEC COMMANDSTRING_SEC
%x INTERFACEEQUATION_SEC CONTACTEQUATION_SEC REGIONEQUATION_SEC
%%
<*>#[^\n]*          ;

<*>[ \t\r]+         ;

<COMMANDSTRING_SEC>\n {
  ++dsDevsimParse::meshlineno;
  yy_pop_state();
  return COMMAND_EOL;
            }

<*>[\n] {++dsDevsimParse::meshlineno;}

<*>{float1}|{float2}|{float3}    {Devsimlval.dval = atof(yytext); return FLOAT;}

<*>{integer}   {Devsimlval.dval = atof(yytext); Devsimlval.ival = atof(yytext); return INT;}

<INITIAL>^begin_device {
                yy_push_state(DEVICE_SEC);
                return BEG_DEVICE;
            }

<DEVICE_SEC>^end_device {
                yy_pop_state();
                return END_DEVICE;
            }

<DEVICE_SEC>^begin_coordinates {
                yy_push_state(COORDINATE_SEC);
                return BEG_COORD;
            }

<COORDINATE_SEC>^end_coordinates {
                yy_pop_state();
                return END_COORD;
            }

<DEVICE_SEC>^begin_region {
                yy_push_state(REGION_SEC);
                return BEG_REGION;
            }

<REGION_SEC>^end_region {
                yy_pop_state();
                return END_REGION;
            }

<REGION_SEC,CONTACT_SEC,INTERFACE_SEC>^begin_nodes {
                yy_push_state(NODE_SEC);
                return BEG_NODE;
            }

<NODE_SEC>^end_nodes {
                yy_pop_state();
                return END_NODE;
            }

<REGION_SEC,CONTACT_SEC,INTERFACE_SEC>^begin_edges {
                yy_push_state(EDGE_SEC);
                return BEG_EDGE;
            }

<EDGE_SEC>^end_edges {
                yy_pop_state();
                return END_EDGE;
            }

<REGION_SEC,CONTACT_SEC,INTERFACE_SEC>^begin_triangles {
                yy_push_state(TRIANGLE_SEC);
                return BEG_TRIANGLE;
            }

<TRIANGLE_SEC>^end_triangles {
                yy_pop_state();
                return END_TRIANGLE;
            }

<REGION_SEC>^begin_tetrahedra {
                yy_push_state(TETRAHEDRON_SEC);
                return BEG_TETRAHEDRON;
            }

<TETRAHEDRON_SEC>^end_tetrahedra {
                yy_pop_state();
                return END_TETRAHEDRON;
            }

<DEVICE_SEC>^begin_contact {
                yy_push_state(CONTACT_SEC);
                return BEG_CONTACT;
            }

<CONTACT_SEC>^end_contact {
                yy_pop_state();
                return END_CONTACT;
            }

<REGION_SEC>^begin_node_solution {
                //// Deprecated and needs to be removed
                yy_push_state(NODESOL_SEC);
                return BEG_NODESOL;
            }

<NODESOL_SEC>^end_node_solution {
                //// Deprecated and needs to be removed
                yy_pop_state();
                return END_NODESOL;
            }

<REGION_SEC>^begin_edge_solution {
                //// Deprecated and needs to be removed
                yy_push_state(EDGESOL_SEC);
                return BEG_EDGESOL;
            }

<EDGESOL_SEC>^end_edge_solution {
                //// Deprecated and needs to be removed
                yy_pop_state();
                return END_EDGESOL;
            }

<REGION_SEC>^begin_node_model {
                //// Deprecated and needs to be removed
                yy_push_state(NODEMODEL_SEC);
                return BEG_NODEMODEL;
            }

<NODEMODEL_SEC>^end_node_model {
                //// Deprecated and needs to be removed
                yy_pop_state();
                return END_NODEMODEL;
            }

<REGION_SEC>^begin_edge_model {
                //// Deprecated and needs to be removed
                yy_push_state(EDGEMODEL_SEC);
                return BEG_EDGEMODEL;
            }

<EDGEMODEL_SEC>^end_edge_model {
                //// Deprecated and needs to be removed
                yy_pop_state();
                return END_EDGEMODEL;
            }

<REGION_SEC>^begin_triangle_edge_model {
                //// Deprecated and needs to be removed
                yy_push_state(TRIANGLEEDGEMODEL_SEC);
                return BEG_TRIANGLEEDGEMODEL;
            }

<TRIANGLEEDGEMODEL_SEC>^end_triangle_edge_model {
                //// Deprecated and needs to be removed
                yy_pop_state();
                return END_TRIANGLEEDGEMODEL;
            }

<REGION_SEC>^begin_tetrahedron_edge_model {
                //// Deprecated and needs to be removed
                yy_push_state(TETRAHEDRONEDGEMODEL_SEC);
                return BEG_TETRAHEDRONEDGEMODEL;
            }

<TETRAHEDRONEDGEMODEL_SEC>^end_tetrahedron_edge_model {
                //// Deprecated and needs to be removed
                yy_pop_state();
                return END_TETRAHEDRONEDGEMODEL;
            }

<INTERFACE_SEC>^begin_interface_node_model {
                //// Deprecated and needs to be removed
                yy_push_state(INTERFACENODEMODEL_SEC);
                return BEG_INTERFACENODEMODEL;
            }

<INTERFACENODEMODEL_SEC>^end_interface_node_model {
                //// Deprecated and needs to be removed
                yy_pop_state();
                return END_INTERFACENODEMODEL;
            }

<INTERFACE_SEC>^begin_equation {
                yy_push_state(INTERFACEEQUATION_SEC);
                return BEG_INTERFACEEQUATION;
            }

<INTERFACEEQUATION_SEC>^end_equation {
                yy_pop_state();
                return END_INTERFACEEQUATION;
            }

<CONTACT_SEC>^begin_equation {
                yy_push_state(CONTACTEQUATION_SEC);
                return BEG_CONTACTEQUATION;
            }

<CONTACTEQUATION_SEC>^end_equation {
                yy_pop_state();
                return END_CONTACTEQUATION;
            }

<REGION_SEC>^begin_equation {
                yy_push_state(REGIONEQUATION_SEC);
                return BEG_REGIONEQUATION;
            }

<REGIONEQUATION_SEC>^end_equation {
                yy_pop_state();
                return END_REGIONEQUATION;
            }

<NODEMODEL_SEC,EDGEMODEL_SEC,TRIANGLEEDGEMODEL_SEC,TETRAHEDRONEDGEMODEL_SEC>^DATA {
              return DATASECTION;
            }

<NODEMODEL_SEC,EDGEMODEL_SEC,TRIANGLEEDGEMODEL_SEC,TETRAHEDRONEDGEMODEL_SEC>^BUILTIN {
              return BUILTINSECTION;
            }

<NODEMODEL_SEC,EDGEMODEL_SEC,TRIANGLEEDGEMODEL_SEC,TETRAHEDRONEDGEMODEL_SEC>^DATAPARENT {
              return DATAPARENTSECTION;
            }

<NODEMODEL_SEC,EDGEMODEL_SEC,TRIANGLEEDGEMODEL_SEC,TETRAHEDRONEDGEMODEL_SEC>^UNIFORM {
              return UNIFORMSECTION;
            }

<NODEMODEL_SEC,EDGEMODEL_SEC,TRIANGLEEDGEMODEL_SEC,TETRAHEDRONEDGEMODEL_SEC,INTERFACENODEMODEL_SEC,REGIONEQUATION_SEC,CONTACTEQUATION_SEC,INTERFACEEQUATION_SEC>^COMMAND[ \t]+ {
              yy_push_state(COMMANDSTRING_SEC);
              return COMMANDSECTION;
            }

 /*
<COMMANDSTRING_SEC>[^\n\{\}\[\]]+ {
              //// try to make sure we can't evaluate []{} in tcl interpreter
              yy_pop_state();
              Devsimlval.str = yytext;
              return COMMANDSTRING;
            }
*/


<COMMANDSTRING_SEC>-[a-z][a-z_0-9]* {
               Devsimlval.str = &(yytext[1]);
               return COMMAND_OPTION;
            }

<DEVICE_SEC>^begin_interface {
                yy_push_state(INTERFACE_SEC);
                return BEG_INTERFACE;
            }

<INTERFACE_SEC>^end_interface {
                yy_pop_state();
                return END_INTERFACE;
            }


<*>([A-Za-z]|{UTF8})([A-Za-z_0-9:@]|{UTF8})*  {
                Devsimlval.str = yytext;
                return WORD;
            }

<*>\"[^\"]*\" {
              Devsimlval.str = yytext;
              Devsimlval.str = Devsimlval.str.substr(1, Devsimlval.str.size() -2);
              return WORD;
           }

<*>.   {return yytext[0];}
%%

int Devsimparse();

namespace dsDevsimParse {
bool LoadMeshes(const std::string &fname, std::string &errorString)
{
    bool ret = false;
    dsDevsimParse::errors.clear();
    dsDevsimParse::meshlineno = 1;

    Devsimin = fopen(fname.c_str(), "r");

    int retval = 1;

    if ( !Devsimin )
    {
        std::ostringstream os;
        os << "Could not open file " << fname << "\n";
        errorString += os.str();
    }
    else
    {
        Devsim_switch_to_buffer(Devsim_create_buffer( Devsimin, YY_BUF_SIZE ) );
        retval = Devsimparse();
        yy_delete_buffer( YY_CURRENT_BUFFER );
    }


    ret = !retval;

    dsDevsimParse::DeletePointers();
    errorString += dsDevsimParse::errors;
    return ret;
}
}

