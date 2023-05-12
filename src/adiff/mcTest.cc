/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "mcTest.hh"
#include "mcModelCompiler.hh"
#include "EquationObject.hh"
#include "UserFunc.hh"
#include "mcModel.hh"
#include "dsAssert.hh"

#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <iomanip>
#include <fstream>
#include <cstdio>

using std::cerr;
using std::endl;
using std::ostringstream;
using std::ofstream;
using std::string;

enum {EQNNUMBER = 0, ISGROUND, SOLUTION, MODEL, RHSEVAL, DERIV, MATRIX};

namespace {
std::list<std::string> TotalNodeList;
}
void dsAssert_(bool x, const std::string &msg)
{
    if (!x)
    {
       std::cerr << msg << std::endl;
        exit(-1);
    }
}

void print_hh (ofstream &);
void print_cc(ofstream &, const string &);

void GenerateEquationDerivatives();
void PrintModelList();
void CreateTotalNodeList();

enum assembletype_t {DCASSEMBLE, ACASSEMBLE};
void PrintAssemblyRoutine(ofstream &, assembletype_t);

/**
  Starts parser
  Reads in script
  Creates output file
  */
int main(int argc, char *argv[]) {

// fix incorrect Microsoft Visual C++ formatting
// https://connect.microsoft.com/VisualStudio/feedback/details/1368280
#if 0
#if _WIN32
    _set_output_format(_TWO_DIGIT_EXPONENT);
#endif
#endif

//   YY_BUFFER_STATE yyb = nullptr;
/*
   if (argc==2)
   {
      yyb = mc_scan_string(argv[1]);
   }
*/

#if 0
    {
        Eqo::CreateUserFunc("tanh", 1);
        Eqo::CreateUserFunc("sin", 1);
        Eqo::CreateUserFunc("cos", 1);
        Eqo::UserDiffInfoVec udiv;
        std::string arg1("@x");
        std::vector<std::string> args;
        args.push_back(arg1);

        Eqo::EqObjPtr One = Eqo::con(1.0);
        Eqo::EqObjPtr X = Eqo::var(arg1);
        std::vector<Eqo::EqObjPtr> ufa;
        ufa.push_back(X);
        Eqo::EqObjPtr tanh = Eqo::EqObjPtr(new Eqo::UserFunc("tanh", ufa));
        Eqo::EqObjPtr Sin = Eqo::EqObjPtr(new Eqo::UserFunc("sin", ufa));
        Eqo::EqObjPtr Cos = Eqo::EqObjPtr(new Eqo::UserFunc("cos", ufa));


        Eqo::EqObjPtr func = One - Eqo::pow(tanh,Eqo::con(2));
        udiv.push_back(std::make_pair(arg1,func));
        Eqo::SetUserFuncDiffs("tanh", udiv);
        udiv.clear();
        udiv.push_back(std::make_pair(arg1,Cos));
        Eqo::SetUserFuncDiffs("sin", udiv);
        udiv.clear();
        udiv.push_back(std::make_pair(arg1,Eqo::con(-1)*Sin));
        Eqo::SetUserFuncDiffs("cos", udiv);

    }
#endif

   int retval = mcparse();
/*
   if (yyb != nullptr)
      mc_delete_buffer(yyb);
*/

   if (retval != 0)
      return retval;

   dsAssert(EquationToNodeMap.size() == EquationList.size(), "UNEXPECTED");
   dsAssert(EquationToNodeMap.size() == (ExternalNodeList.size() + InternalNodeList.size()), "UNEXPECTED");

   if (ClassName.empty())
   {
      cerr << "title not set\n";
      exit(-1);
   }


   std::string hFileName = ClassName + ".hh";
   std::string cFileName = ClassName + ".cc";

   ofstream headerFile(hFileName.c_str(), std::ios::out );
   if (!headerFile)
   {
      cerr << "Could not open header file: " << hFileName << "\n";
      exit(-1);
   }

   ofstream ccFile(cFileName.c_str(), std::ios::out );
   if (!ccFile)
   {
      cerr << "Could not open implementation file: " << cFileName << "\n";
      exit(-1);
   }


    CreateTotalNodeList();
    GenerateEquationDerivatives();

    PrintModelList();

   print_hh(headerFile);
   headerFile.close();

   print_cc(ccFile, hFileName);
   ccFile.close();

   return retval;
}

namespace {
   std::string nodePtrName (const std::string &x) {
      return std::string("node_ptr_") + x;
   }
   // fix this later
   // or just have user have own notation
   std::string parameterListName (const std::string &x) {
      return x;
   }
   std::string nodeNbrName (const std::string &x) {
      return std::string("node_num_") + x;
   }
   // are we dealing with a ground node
   std::string nodeIsGndName (const std::string &x) {
      return std::string("is_gnd_node_") + x;
   }
   std::string EqDerivName (const std::string &x, const std::string &y) {
      return x + "_" + y;
   }

  std::string GetUniqueName()
  {
    static size_t i = 0;
    std::ostringstream os;
    os << "unique" << i++;
    return os.str();
  }

  class SubExpr {
    public:
      SubExpr() : refcount(0) {}
      SubExpr(Eqo::EqObjPtr x)
      {
        refcount = 1;
        equation = x->clone();
      }
      void addCount()
      {
        ++refcount;
      }
      size_t getCount() const
      {
        return refcount;
      }

      Eqo::EqObjPtr getEquation() const
      {
        return equation->clone();
      }
      const std::string &getName() const {
        if (name.empty())
        {
          name = GetUniqueName();
        }
        return name;
      }

    private:
      size_t refcount;
      Eqo::EqObjPtr equation;
      mutable std::string name;
  };

  std::map<std::string, SubExpr> SubExprMap;
  //// We must always scan in the same order
  std::vector<std::string>       SubExprOrdered;

  /// Recursively scan subexpressions and add to map
  /// only do this on unscaled values
  void scanSubexpression(Eqo::EqObjPtr &eq)
  {
    Eqo::EqObjPtr te = eq->getUnscaledValue();
    Eqo::EqObjType t = te->getType();
    //// These are too simple to cache
    if (t == Eqo::MODEL_OBJ || t == Eqo::VARIABLE_OBJ || t == Eqo::CONST_OBJ)
    {
      return;
    }

    const std::string &eqstring = te->stringValue();

    std::map<std::string, SubExpr>::iterator it = SubExprMap.find(eqstring);
    if (it != SubExprMap.end())
    {
      (it->second).addCount();
    }
    else
    {
      SubExprMap.insert(std::make_pair(eqstring, te));
      SubExprOrdered.push_back(eqstring);
    }
    //// Since this is the first time, we will look for temporaries which may be shared as well
    //// If they are truly unique, they will only be visited once since the parent will only be descended into once
    //// Keep going  TODO: see what we are doing in devsim
    std::vector<Eqo::EqObjPtr> vec = te->getArgs();
    for (size_t i = 0; i < vec.size(); ++i)
    {
      scanSubexpression(vec[i]);
    }
  }

    void createSubexpressions()
    {
        SubExprMap.clear();

        /*** Start with whole model list, assume all derivatives created ***/
       typedef std::list<std::pair<std::string, Eqo::EqObjPtr> >::iterator StrEqListIt;
       StrEqListIt semit, semend;
        semit = ModelList.begin();
        semend = ModelList.end();
        /*** start adding to structure.  ***/
        for ( ; semit != semend; ++semit)
        {
          /// what if two models have the same definition?
          scanSubexpression(semit->second);
#if 0
          std::vector<Eqo::EqObjPtr> vec = semit->second->getArgs();
          for (size_t i = 0; i < vec.size(); ++i)
          {
            scanSubexpression(vec[i]);
          }
#endif
        }

        /*** now scan, and if ref count is greater than 1, then create unique name and substitute ***/
        std::vector<std::string> final_list;
        final_list.reserve(SubExprOrdered.size());
        for (std::vector<std::string>::iterator it = SubExprOrdered.begin(); it != SubExprOrdered.end(); ++it)
        {
          //// This is unique so should not be temporary
          std::map<std::string, SubExpr>::iterator jt = SubExprMap.find(*it);
          if (jt != SubExprMap.end())
          {
            std::cerr << "Ref count " << *it << " " << (jt->second).getCount() << "\n";
            if ((jt->second).getCount() > 1)
            {
              final_list.push_back(*it);
            }
          }
        }

        //// Create temporaries
        for (size_t i = 0; i < final_list.size(); ++i)
        {
          const SubExpr &subexpr = SubExprMap[final_list[i]];
          ModelList.push_back(std::make_pair(subexpr.getName(), subexpr.getEquation()));
        }
        semit = ModelList.begin();
        semend = ModelList.end();
        for ( ; semit != semend; ++semit)
        {
          for (size_t i = 0; i < final_list.size(); ++i)
          {
            const SubExpr &subexpr = SubExprMap[final_list[i]];
            /// make sure we don't substitute something for itself
            if (subexpr.getName() != semit->first)
            {
              semit->second = (semit->second)->subst(final_list[i], Eqo::mod(subexpr.getName()));
            }
          }
        }
    }

   /*
      Only print equations
      Called recursively.  Refactor, if possible, using iteration.
   */
   std::string printForEquation(std::map<std::string, bool > &wasProcessed, Eqo::EqObjPtr &eq)
   {
      ostringstream out;

      //// It this equation is only a reference to a model, and the string was already printout out,
      //// then just resturn an empty string
      if (eq->getType() == Eqo::MODEL_OBJ) {
         const std::string mname = eq->stringValue();
         if (wasProcessed[mname]) {
            return out.str();
         }
      }

      std::set<std::string> mset;
      if (eq->getType() == Eqo::MODEL_OBJ) {
         //// Get all models referenced by this model
         const std::string mname = eq->stringValue();
         mset = findInModelList(mname)->getReferencedType(Eqo::MODEL_OBJ);
      }
      else {
         //// Get all models referenced by this object
         mset = eq->getReferencedType(Eqo::MODEL_OBJ);
      }

      std::set<std::string>::iterator it =mset.begin();
      std::set<std::string>::iterator end=mset.end();
      for ( ; it != end; ++it)
      {
         // Model was processed, we are done
         if (wasProcessed[*it])
         {
            continue;
         }

         //// print out all models referenced by this equation
         Eqo::EqObjPtr x = findInModelList(*it);
         out << printForEquation( wasProcessed, x);
         out <<
"   const double " << *it << " = "      << x << ";\n";
         wasProcessed[*it] = true;

      }

      //// If we are a model, go ahead and print out our equation
      if (eq->getType() == Eqo::MODEL_OBJ) {
         const std::string mname = eq->stringValue();
         out <<
            "   const double " << eq << " = " << findInModelList(mname) << ";\n";
         wasProcessed[mname] = true;
      }
      return out.str();
   }
}

//// creates total node list
//// prints out resulting nodes
void CreateTotalNodeList()
{
   typedef std::list<std::string>::iterator strsetit;
   strsetit sit, send;

   for ( sit=ExternalNodeList.begin(); sit != ExternalNodeList.end(); ++sit)
      TotalNodeList.push_back(*sit);
   for ( sit=InternalNodeList.begin(); sit != InternalNodeList.end(); ++sit)
      TotalNodeList.push_back(*sit);

   cerr << "External Nodes:\n";
   sit=ExternalNodeList.begin();
   send=ExternalNodeList.end();

   ostringstream nodeDecls;
   for ( ; sit != send; ++sit)
   {
      cerr << "\t" << (*sit) << "\n";
   }

   cerr << endl;

   cerr << "Internal Nodes:\n";
   sit=InternalNodeList.begin();
   send=InternalNodeList.end();
   for ( ; sit != send; ++sit)
   {
      cerr << "\t" << (*sit) << "\n";
   }
   cerr << endl;
}

void PrintModelList()
{
   typedef std::list<std::pair<std::string, Eqo::EqObjPtr> >::iterator StrEqListIt;
   StrEqListIt semit, semend;

   cerr << "Models:\n";
   semit=ModelList.begin();
   semend=ModelList.end();
   for ( ; semit != semend; ++semit)
   {
      cerr << "\t" << (*semit).first << "\t" <<
         (*semit).second << "\n";
   }
   cerr << "\n";

}

/*
 *  Prints diagnostics to screen.
 *  Prints out derivatives of equations
 *  Implicitly creates missing models for derivatives
 */
void GenerateEquationDerivatives()
{
   typedef std::map<std::string, std::pair<Eqo::EqObjPtr, Eqo::EqObjPtr> >::iterator StrEqEqMapIt;

   StrEqEqMapIt seemit, seemend;

   typedef std::list<std::string>::iterator strsetit;
   strsetit sit, send;



   cerr << "Equations:\n";
   seemit=EquationList.begin();
   seemend=EquationList.end();
   for ( ; seemit != seemend; ++seemit)
   {
      cerr << "    " << (*seemit).first << "\n"
          "    dc  "  << (*seemit).second.first << "\n"
          "    td  "  << (*seemit).second.second << endl;

      cerr << "  Derivatives wrt External Nodes:" << endl;
      sit=ExternalNodeList.begin();
      send=ExternalNodeList.end();
      for ( ; sit != send; ++sit)
      {
          /* Reuse message from simplify will print to stderr */
          std::string tstr = Eqo::Simplify(Eqo::diff((*seemit).second.first,Eqo::var(*sit)))->stringValue();
          cerr << "  dc," << (*sit);
          cerr << "  " << tstr <<  endl;

          tstr = Eqo::Simplify(Eqo::diff((*seemit).second.second,Eqo::var(*sit)))->stringValue();
          cerr << "  td," << (*sit);
          cerr << "  " << tstr <<  endl;
      }
      cerr << endl;

      cerr << "Derivatives wrt Internal Nodes:" << endl;
      sit=InternalNodeList.begin();
      send=InternalNodeList.end();
      for ( ; sit != send; ++sit)
      {
          /* Reuse message from simplify will print to stderr */
          std::string tstr = Eqo::Simplify(Eqo::diff((*seemit).second.first,Eqo::var(*sit)))->stringValue();
          cerr << "  dc," << (*sit);
          cerr << "  " << tstr <<  "\n";

          tstr = Eqo::Simplify(Eqo::diff((*seemit).second.second,Eqo::var(*sit)))->stringValue();
          cerr << "  td," << (*sit);
          cerr << "  " << tstr <<  "\n";
      }
      cerr << endl;

      }
   cerr << "\n";
}

void print_hh(ofstream &out) {
   typedef std::list<std::string>::iterator listit;
   listit lit, lend;
         /*
            Includes
         */
   out << "#include \"InstanceModel.hh\"\n";
   out << "#include \"CircuitNode.hh\"\n";
   out << "#include \"NodeKeeper.hh\"\n";
   out << "#include \"MatrixEntries.hh\"\n";
   out << "#include <cmath>\n";

   /// Make this optional compilation for user models
   out << "\n" << "class " << ClassName << ";\n"
          "extern \"C\" InstanceModel *" << ClassName << "_create"
          " (NodeKeeper *, const std::string &name, const std::vector<std::string> &nodelist);\n";

   out <<
   "class " << ClassName << " : public InstanceModel {\n"
   "   public:\n"
   "        " << ClassName << "(\n"
   "          NodeKeeper *nk,\n" <<
   "          const char *name,\n";

   /*
      External Node list
   */
   size_t i=1;
   for (lit = ExternalNodeList.begin(); lit != ExternalNodeList.end(); ++lit)
   {
      if (i != 1)
         out << ",\n"; // need to start next line

      out << "          const char *n" << i;
      ++i;
   }
   out << ");\n\n";
out <<
"       void assembleDC_impl(const NodeKeeper::Solution &, dsMath::RealRowColValueVec<double> &, std::vector<std::pair<int, double> > &);\n"

"       void assembleTran_impl(const double scl, const NodeKeeper::Solution &sol, dsMath::RealRowColValueVec<double> *mat, std::vector<std::pair<int, double> > &rhs);\n"
"       bool addParam(const std::string &, double);\n"
"    private:\n"
"       " << ClassName << "();\n"
"       " << ClassName << "(const " << ClassName << " &);\n"
"       " << ClassName << " operator=(const " << ClassName << " &);\n"
"\n";

   /*
      print node ptr names
      */
out <<
"       //External Nodes\n";
   for (lit = ExternalNodeList.begin(); lit != ExternalNodeList.end(); ++lit)
   {
out <<
"       CircuitNodePtr "  << nodePtrName(*lit) << ";\n";
   }
out << "\n"
"       //Internal Nodes\n";
   for (lit = InternalNodeList.begin(); lit != InternalNodeList.end(); ++lit)
   {
out <<
"       CircuitNodePtr "  << nodePtrName(*lit) << ";\n";
   }

   // Initial Implementation has default parameters for Model
   // Add bounds checking as well as modelPtr later
out << "\n"
"       //Parameter List\n";
   typedef std::map<std::string, std::pair<std::string, double> >::iterator Plistit;
   {
     std::ostringstream os;
     for (Plistit it = ParameterList.begin(); it != ParameterList.end(); ++it)
     {
        os <<
"       double " << std::setw(15) << std::left << parameterListName(it->first) +
              ";" <<  "//" << std::setw(20) << it->second.first << " " << std::scientific << it->second.second << "\n";

     }
     out << os.str();
    }

   /*
      End of Class Declaration
   */
out << "};\n";

}

// Constructor Definition
void print_cc(ofstream &out, const string &headerFile)
{
  size_t mlsize;
  do
  {
    mlsize = ModelList.size();
    createSubexpressions();
  }
  while (mlsize != ModelList.size());

   out << "#include \"" << headerFile << "\"\n";
   out << "#include <cmath>\n";

   typedef std::list<std::string>::iterator listit;
   listit lit, lend;

   out <<
ClassName << "::" << ClassName << "( NodeKeeper *nk, const char *name,\n";
   int i=1;
   for (lit = ExternalNodeList.begin(); lit != ExternalNodeList.end(); ++lit)
   {
      if (i != 1)
         out << ",\n"; // need to start next line

      out << "          const char *n" << i;
      ++i;
   }
   out << ") : InstanceModel(nk, name)\n"
"{\n";
   i=1;
   for (lit = ExternalNodeList.begin(); lit != ExternalNodeList.end(); ++lit)
   {
      out <<
"       "  << nodePtrName(*lit) << "= this->AddCircuitNode(n" << i <<");\n";
      ++i;
   }
   for (lit = InternalNodeList.begin(); lit != InternalNodeList.end(); ++lit)
   {
      out <<
"       "  << nodePtrName(*lit) << "= this->AddInternalNode(\"" <<  *lit << "\");\n";
      ++i;
   }

   out << "\n"
"       //Parameter List\n";
   typedef std::map<std::string, std::pair<std::string, double> >::iterator Plistit;
   {
      std::ostringstream os;
      for (Plistit it = ParameterList.begin(); it != ParameterList.end(); ++it)
      {
         os <<
"       " << std::setw(15) << std::left << parameterListName(it->first) <<
            " = " << std::scientific << it->second.second << ";\n";
      }
      out << os.str();
   }
out << "}\n";


    PrintAssemblyRoutine(out, DCASSEMBLE);
    PrintAssemblyRoutine(out, ACASSEMBLE);

   out <<
"bool " << ClassName << "::addParam(const std::string &str, double val)\n"
"{\n";
//   typedef std::map<std::string, std::pair<std::string, double> >::iterator Plistit;
   out << "    bool ret = false;\n";
   bool begif = false;
   for (Plistit it = ParameterList.begin(); it != ParameterList.end(); ++it)
   {
    if (begif)
    {
        out << "    else if";
    }
    else
    {
        out << "    if";
        begif = true;
    }

    out <<
" (" << "str == \"" << parameterListName(it->first) << "\")\n"
"    {\n" <<
"        " << parameterListName(it->first) << " = val;\n"
"        ret = true;\n"
"    }\n";
   }
    out <<
"    return ret;\n"
"}\n";

   /// Make this optional compilation for user models
   out <<
          "\nextern \"C\" InstanceModel *" << ClassName << "_create"
          " (NodeKeeper *nk, const std::string &name, const std::vector<std::string> &nodelist)"
          " {\n"
          " return new " << ClassName << "(nk, name.c_str()";

   i=0;
   for (lit = ExternalNodeList.begin(); lit != ExternalNodeList.end(); ++lit)
   {
      out << ", nodelist[" << i << "].c_str()";
      ++i;
   }
   out << ");\n";
   out << "\n}" << endl;
}


void PrintAssemblyRoutine(ofstream &out, assembletype_t atype)
{

   typedef std::list<std::string>::iterator listit;
   listit lit, lend;
/*
   Equations stamp
   i=0 conductance
   i=1 charge
 */
   out << "\n";

/**
  This section is for the equation assembly
  */
// Need to make sure we get all the models defined in the right order
   // rely on default initialization to zero (false)
   std::map<std::string, bool> wasModelProcessed;

if (atype == DCASSEMBLE)
{
   out <<
"void " << ClassName << "::assembleDC_impl(const NodeKeeper::Solution &sol, dsMath::RealRowColValueVec<double> &mat, std::vector<std::pair<int, double> > &rhs)\n";
}
else if (atype == ACASSEMBLE)
{
   out <<
"void " << ClassName << "::assembleTran_impl(const double scl, const NodeKeeper::Solution &sol, dsMath::RealRowColValueVec<double> *mat, std::vector<std::pair<int, double> > &rhs)\n";
}

   out <<
"{\n";
   // go through all the equations
   typedef std::map<std::string, std::pair<Eqo::EqObjPtr, Eqo::EqObjPtr> >::iterator StrEqEqMapIt;
   StrEqEqMapIt seemit, seemend;
   ostringstream bos[7];
   seemend=EquationList.end();
   for (seemit=EquationList.begin(); seemit != EquationList.end(); ++seemit)
   {
      Eqo::EqObjPtr pt;
      if (atype == DCASSEMBLE)
         pt = seemit->second.first;
      else if (atype == ACASSEMBLE)
         pt = seemit->second.second;

      if (pt ->isZero())
         continue;

      std::string tmpname=(EquationToNodeMap[seemit->first]);

      bos[EQNNUMBER] <<
"   const size_t " << nodeNbrName(tmpname) << " = " << nodePtrName(tmpname) << "->getNumber();\n";

      bos[ISGROUND] <<
"   const bool   " << nodeIsGndName(tmpname) << " = " << nodePtrName(tmpname) << "->isGROUND();\n";

      bos[SOLUTION] <<
"   const double " << tmpname << " = (" << nodeIsGndName(tmpname) << ") ? 0.0 : " "sol[" << nodeNbrName(tmpname) << "];\n";


//       out << "calling print for equation!\n";
//       out << seemit->second.first << "\n";
      bos[MODEL] << printForEquation(wasModelProcessed, pt);

      if (atype == DCASSEMBLE)
      {
        bos[MODEL] << "   const double " << seemit->first << " = ";
        bos[MODEL] << pt << ";\n";
      }
      else if (atype == ACASSEMBLE)
      {
        bos[MODEL] << "   const double " << seemit->first << " = ";
        bos[MODEL] << "scl *";
        bos[MODEL] << pt << ";\n";
      }

      bos[RHSEVAL] <<
   "   if (!" << nodeIsGndName(tmpname) << ")\n"
   "      rhs.push_back(std::make_pair(" << nodeNbrName(tmpname)  << ", " << seemit->first << "));\n";

      bos[MATRIX] <<
"   if (!" << nodeIsGndName(tmpname) << ")\n"
"   {\n";

   for (lit = TotalNodeList.begin(); lit != TotalNodeList.end(); ++lit)
   {
      Eqo::EqObjPtr ptdiff = Eqo::Simplify(Eqo::diff( pt, Eqo::var(*lit)));
      if (ptdiff->isZero())
         continue;


      // use the same list as before to make sure no redundant models
      bos[DERIV] << printForEquation(wasModelProcessed, ptdiff);

      std::string nm = EqDerivName(seemit->first, *lit);
      bos[DERIV] <<
"   const double " << nm;

      std::string op1;
      if (atype == DCASSEMBLE)
      {
         bos[DERIV] << " = ";
         op1 = ".";
      }
      else if (atype == ACASSEMBLE)
      {
         bos[DERIV] << " = scl * ";
         op1 = "->";
      }
      bos[DERIV] << ptdiff << ";\n";

      if (tmpname != *lit)
      {
         bos[MATRIX] <<
"      if (!" << nodeIsGndName(*lit) << ")\n"
"   ";
      }

      bos[MATRIX] <<
"      mat" << op1 << "push_back(dsMath::RealRowColVal<double>(" << nodeNbrName(tmpname) << ","
         << nodeNbrName(*lit) << ", " << nm << "));\n";
   }
   bos[MATRIX] <<
"   }\n";
   }
   bos[EQNNUMBER] << "\n";
   bos[ISGROUND] << "\n";
   bos[SOLUTION] << "\n";
   bos[MODEL] << "\n";
   bos[RHSEVAL] << "\n";
   bos[DERIV] << "\n";
   bos[MATRIX] << "\n";

   out << bos[EQNNUMBER].str();
   out << bos[ISGROUND].str();
   out << bos[SOLUTION].str();
   out << bos[MODEL].str();
   out << bos[RHSEVAL].str();

   if (atype == ACASSEMBLE)
   {
      out << "\n"
"   if (mat == nullptr)\n"
"      return;\n";
   }

   out << "\n";
   out << bos[DERIV].str();
   out << bos[MATRIX].str();
      out <<
"}\n\n";

}

