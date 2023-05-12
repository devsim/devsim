%{
/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/


#include "mcTest.hh"
#include "EquationObject.hh"
#include "mcModel.hh"
#include "UserFunc.hh"
#include "mcModelCompiler.hh"
#include "LogicalFunc.hh"
#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <vector>
using std::cerr;
using std::endl;

int mcerror(const char *msg);
namespace {
   bool IsUnique(const std::string & str);
}
%}

/**
   Todo:

   external_node
   internal_node
   link (link equation to either an internal or external nodes.)

   Each routine is precomputed within the routine.

   If a derivative of equation is zero, then it is not added to symbolic.

   For DC assembly,

      only derivative models are calculated for resistive terms

   For AC assembly

      only derivative models are calculated for capactive terms
  */

//%union {}
//    char *sval;
//    double dval;
//}

//%token HELP
%token <dval> INT FLOAT
%token <str> VAR UNARYFUNC BINARYFUNC MOD_CMD MODELNAME EQ_CMD EQUATIONNAME
%token <str> DEFUSERFUNC DECUSERFUNC USERFUNC
%token <str> EXTN_CMD INTN_CMD EXTERNALNODE INTERNALNODE
%token <str> HELP DDT
%token <str> PARAM_CMD PARAMNAME
%token <str> STRING LINK_CMD
%token <str> TITLE
%token <str> LT GT EQ NEQ LTE GTE NOT AND OR
%type  <eqptr> number expression modelptr intnodeptr extnodeptr paramptr;
%type  <eqptr> userfunc;
%type <str> modelcmd equationcmd commands equationname nodename;
%type <str> extnodecmd intnodecmd;
%type <str> paramcmd linkcmd titlecmd;
%type <str> userfunccmd;
%type  <vararg> vararg
%type  <varobj> varobj

%left OR
%left AND
%left EQ NEQ
%left LT LTE GT GTE
%left '-' '+'
%left '*' '/'
%right '^'
%right UMINUS UPLUS NOT

%%
all : | all commands
    ;

commands : modelcmd | equationcmd | extnodecmd | intnodecmd |
	   paramcmd | linkcmd | titlecmd | userfunccmd;

titlecmd : TITLE VAR ';'
	 {
		$$ = $2;
		if (ClassName.empty())
		{
		  ClassName = $$;
		}
		else {
			mcerror("Cannot change title");
			exit(-1);
		}
	 }
	 ;

modelcmd : MOD_CMD VAR expression ';' {$$=$2;
	      // need explicit error saying that using another name is bad
	      // now only a syntax error
	       cerr << "adding model: " << $2 << " " << $3 <<  "\n";
	       if (IsUnique($2))
		  ModelList.push_back(make_pair($2,$3->Simplify()));
	       }
	       ;

equationcmd : EQ_CMD VAR expression DDT expression ';' {$$=$2;
	       cerr << "adding equation: " << $2 << " " << $3;
	       if (IsUnique($2))
		  EquationList[$2].first = $3;
		  EquationList[$2].second = $5; cerr << " ddt " << $5 << "\n";} ;

extnodecmd : EXTN_CMD VAR ';' {$$=$2;
	       cerr << "External Node: " << $$ << "\n";
	       if (IsUnique($2))
		  ExternalNodeList.push_back($2);
	       };

intnodecmd : INTN_CMD VAR ';' {$$=$2;
	       cerr << "Internal Node: " << $$ << "\n";
	       if (IsUnique($2))
		  InternalNodeList.push_back($2);
	       };

paramcmd : PARAM_CMD VAR STRING FLOAT ';' {$$=$2;
	       cerr << "Parameter Node: " << $$ << " "
		  << $3 << " " << $4 << "\n";
	       if (IsUnique($2))
		  ParameterList[$2]=std::make_pair($3,$4);
	       };

linkcmd : LINK_CMD equationname nodename ';' {
	     if (EquationToNodeMap.count($2) > 0)
	     {
		mcerror("Cannot relink equation name");
		exit(-1);
	     }
	     typedef std::map<std::string, std::string>::iterator ittd;
	     ittd iter = EquationToNodeMap.begin(), end = EquationToNodeMap.end();
	     for (;iter != end; ++iter)
	     {
		if ((*iter).second == $3)
		{
		   mcerror("Cannot relink node name");
		   exit(-1);
		}
	     }
	     EquationToNodeMap[$2]=$3;
	  };

expression :
	   expression EQ  expression {$$ = Eqo::EqObjPtr(new Eqo::BinaryLogical($2, $1, $3 ));} |
	   expression NEQ expression {$$ = Eqo::EqObjPtr(new Eqo::BinaryLogical($2, $1, $3 ));} |
	   expression LT  expression {$$ = Eqo::EqObjPtr(new Eqo::BinaryLogical($2, $1, $3 ));} |
	   expression LTE expression {$$ = Eqo::EqObjPtr(new Eqo::BinaryLogical($2, $1, $3 ));} |
	   expression GT  expression {$$ = Eqo::EqObjPtr(new Eqo::BinaryLogical($2, $1, $3 ));} |
	   expression GTE expression {$$ = Eqo::EqObjPtr(new Eqo::BinaryLogical($2, $1, $3 ));} |
	   expression AND expression {$$ = Eqo::EqObjPtr(new Eqo::BinaryLogical($2, $1, $3 ));} |
	   expression OR  expression {$$ = Eqo::EqObjPtr(new Eqo::BinaryLogical($2, $1, $3 ));} |
	   NOT expression {$$ = Eqo::EqObjPtr(new Eqo::UnaryLogical($1, $2));} |
	   expression '^' expression {$$ = Eqo::getBinaryFuncPtr("pow")($1, $3);} |
	   expression '+' expression {$$ = $1 + $3;} |
	   expression '-' expression {$$ = $1 - $3;} |
	   expression '*' expression {$$ = $1 * $3;} |
	   expression '/' expression {$$ = $1 / $3;} |
	   '-' expression %prec UMINUS {
		$$ = Eqo::con(-1) * $2;
		if ($2->getType() == Eqo::CONST_OBJ)
		{
		    $$ = $$->Simplify();
		}
	   } |
	   '+' expression %prec UPLUS  {$$ = $2;} |
	   UNARYFUNC '(' varobj ')' {
		if ($3.size() == 1)
		{
		    $$ = Eqo::getUnaryFuncPtr($1)($3[0]) ;
		}
		else
		{
		    std::ostringstream os;
		    os << "Function \""
			<< $1 << "\" expects 1 argument and you provided " <<  $3.size();
		    std::string foo = os.str();
		    yyerror(foo.c_str());
		    $$ = Eqo::con(0.0);
		}
	   } |
	   BINARYFUNC '(' varobj ')' {
		if ($3.size() == 2)
		{
		    $$ = Eqo::getBinaryFuncPtr($1)($3[0], $3[1]) ;
		}
		else
		{
		    std::ostringstream os;
		    os << "Function \""
			<< $1 << "\" expects 2 arguments and you provided " <<  $3.size();
		    std::string foo = os.str();
		    yyerror(foo.c_str());
		    $$ = Eqo::con(0.0);
		}
	    } |
	   '(' expression ')' { $$ = $2;} |
	   userfunc |
	   number |
	   intnodeptr |
	   extnodeptr |
	   modelptr |
	   paramptr |
	   VAR {
	    /*
		This may cause lots of trouble later on, but is necessary
		for dummy arguments to functions
	    */
	      $$=Eqo::var($1);
	   /* $$=Eqo::con(0.0); mcerror("Undefined name in expression"); exit(-1);*/
	   } |
	   equationname { $$=Eqo::con(0.0); mcerror("Cannot use equation name in expression"); exit(-1);}
	   ;

userfunccmd :
	    DEFUSERFUNC VAR '(' vararg ')' ',' varobj ';' {
		const size_t nargs = $4.size();
		if ($4.size() != $7.size())
		{
		    std::ostringstream os;
		    os << "argument list not the same size as the object list "
			<< $4.size() << " args versus " <<  $7.size() << " objects";
		    std::string foo = os.str();
		    yyerror(foo.c_str());
		}
		else
		{
		    Eqo::CreateUserFunc($2, nargs);
		    Eqo::UserDiffInfoVec udiv;
		    std::vector<Eqo::EqObjPtr> f;

		    // create special reserved names
		    for (size_t i = 0; i < nargs; ++i)
		    {
			std::string foo = $4[i];
			$4[i] = "@" + foo; // create reserved name
			for (size_t j = 0; j < nargs; ++j)
			{
			    $7[j]=$7[j]->subst(foo, Eqo::var($4[i]));
			}
		    }
		    for (size_t i = 0; i < nargs; ++i)
		    {
    //		    std::cout << $8[i] << std::endl;
			udiv.push_back(std::make_pair($4[i], $7[i]));
			f.push_back(Eqo::var($4[i].substr(1)));
		    }
		    Eqo::SetUserFuncDiffs($2, udiv);
		}
	   } |
	    DEFUSERFUNC USERFUNC '(' vararg ')' ',' varobj ';' {
		const size_t ulen = Eqo::UserFuncMap[$2].size();
		const size_t nargs = $4.size();
		if (ulen != nargs)
		{
		    std::ostringstream os;
		    os << $2 << " has been declared or defined with "
			<< ulen << " argument(s), but is being redefined with " <<
			nargs << " arguments(s).";
		    std::string foo = os.str();
		    yyerror(foo.c_str());
		}
		else if ($4.size() != $7.size())
		{
		    std::ostringstream os;
		    os << "argument list for " << $2 << " is not the same size as the object list. "
			<< $4.size() << " args versus " <<  $7.size() << " objects";
		    std::string foo = os.str();
		    yyerror(foo.c_str());
		}
		else
		{
		// Just like, except for create
//		Eqo::CreateUserFunc($3, nargs);
		    Eqo::UserDiffInfoVec udiv;
		    std::vector<Eqo::EqObjPtr> f;

		    for (size_t i = 0; i < nargs; ++i)
		    {
			udiv.push_back(std::make_pair($4[i], $7[i]));
			f.push_back(Eqo::var($4[i]));
		    }
		    Eqo::SetUserFuncDiffs($2, udiv);
		}
	   } |
	   DECUSERFUNC VAR '(' vararg ')' ';' {
		const size_t nargs = $4.size();
		Eqo::CreateUserFunc($2, $4.size());
		std::vector<Eqo::EqObjPtr> f;
		for (size_t i = 0; i < nargs; ++i)
		{
		    f.push_back(Eqo::var($4[i]));
		}
	   } |
	   DECUSERFUNC USERFUNC '(' vararg ')' ';' {
		std::ostringstream os;
		os << $2 << " has already been declared with "
		    << Eqo::UserFuncMap[$2].size() << " argument(s)";
		std::string foo = os.str();
		yyerror(foo.c_str());
	   }
	   ;


userfunc :
	   USERFUNC '(' varobj ')' {
		const size_t ulen = Eqo::UserFuncMap[$1].size();
		const size_t vlen = $3.size();
		if ( ulen != vlen)
		{
		    std::ostringstream os;
		    os << $1 << " has been declared or defined with "
			<< ulen << " argument(s), but has been supplied with " <<
			vlen << " arguments(s).";
		    std::string foo = os.str();
		    yyerror(foo.c_str());
		    $$ = Eqo::con(0.0);
		}
		else
		{
		    std::vector<Eqo::EqObjPtr> f;
		    const size_t nargs = $3.size();
		    for (size_t i = 0; i < nargs; ++i)
		    {
			f.push_back($3[i]);
		    }
		    $$ = Eqo::EqObjPtr(new Eqo::UserFunc($1, f));
		}
	   };

varobj :    expression {
		std::vector<Eqo::EqObjPtr> foo;
		foo.push_back($1);
		$$ = foo;
	    } |
	    varobj ',' expression {
		$$ = $1;
		$$.push_back($3);
	    };

vararg :    VAR {
		std::vector<std::string> foo;
		foo.push_back($1);
		$$ = foo;
	    } |
	    vararg ',' VAR {
		$$ = $1;
		$$.push_back($3);
	    }
	    ;

number :    INT { $$ = Eqo::con($1);} |
	    FLOAT { $$ = Eqo::con($1);}
	  ;

nodename : INTERNALNODE {$$=$1;} |
	   EXTERNALNODE {$$=$1;};

intnodeptr :    INTERNALNODE { $$ = Eqo::var($1); }
	  ;

extnodeptr :    EXTERNALNODE { $$ = Eqo::var($1); }
	  ;

modelptr    :   MODELNAME { $$ = Eqo::mod($1);};

paramptr    :   PARAMNAME { $$ = Eqo::var($1);};

equationname :   EQUATIONNAME { $$ = $1;};

%%

int mcerror(const char *msg) {
    std::cerr << msg << "\n";
    return 0;
}

namespace {
   // Even fails with an error
   // \todo make more verbose (say the name)
   bool IsUnique(const std::string & str)
   {
      std::string out;
      if (isInModelList(str))
      {
	 out = "Cannot replace existing model name, ";
      }
      else if (EquationList.count(str))
      {
	 out = "Cannot replace existing equation name, ";
      }
      else if (find(ExternalNodeList.begin(), ExternalNodeList.end(), str) != ExternalNodeList.end())
      {
	 out = "Cannot replace existing external node name, ";
      }
      else if (find(InternalNodeList.begin(), InternalNodeList.end(), str) != InternalNodeList.end())
      {
	 out = "Cannot replace existing internal node name, ";
      }
      else if (ParameterList.count(str))
      {
	 out = "Cannot replace existing parameter list name, ";
      }

      if (!out.empty())
      {
	out += str;
	mcerror(out.c_str());
	exit(-1);
	return false;
      }
      return true;
   }
}
