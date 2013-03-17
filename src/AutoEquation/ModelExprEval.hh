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

#ifndef MODELEXPREVAL_HH
#define MODELEXPREVAL_HH
// use a map to look up values in our class
// maybe turn this class into a template for scalars and vectors later on
#include <map>
#include <string>
#include <list>
#include <vector>
class Region;
typedef Region *RegionPtr;


#include "ModelExprData.hh"

namespace MEE {
class ModelExprEval {
    public:
        enum ExpectedType {UNKNOWN = 0, NODE, EDGE, TRIANGLEEDGE, TETRAHEDRONEDGE};
        typedef std::vector<ModelExprData> margv_t;
        typedef std::list<std::string> error_t;
        /// need to generalize to contact and interface equations as well
        typedef const Region * data_ref_t;
        /// we are given a reference to our callbacks
        /// could just as soon be a class which is called back for these variables
        ModelExprEval(data_ref_t &, const std::string &, error_t &);
//      ModelExprEval(data_ref_t &, error_t &, double); // for creation from double
        ~ModelExprEval();

        ModelExprData eval_function(Eqo::EqObjPtr);
    private:
        ModelExprData EvaluateAddType(Eqo::EqObjPtr);
        ModelExprData EvaluateProductType(Eqo::EqObjPtr);
        ModelExprData EvaluateVariableType(Eqo::EqObjPtr);
        ModelExprData EvaluateConstantType(Eqo::EqObjPtr);
        ModelExprData EvaluateModelType(Eqo::EqObjPtr);
        ModelExprData EvaluateIfType(Eqo::EqObjPtr);
        ModelExprData EvaluateIfElseType(Eqo::EqObjPtr);
        ModelExprData EvaluateFunctionType(Eqo::EqObjPtr);
        ModelExprData EvaluateInvalidType(Eqo::EqObjPtr);
        ///  Handles math functions (log, pow, exp, . . .)
        ModelExprData EvaluateExternalMath(const std::string &, margv_t &);
//      ModelExprData createModelExprData(Eqo::EqObjPtr);
        data_ref_t              &data_ref; // reference to data for variables
        const std::string       model;     // model being evaluated
        error_t                 &errors;
        std::vector<size_t>     indexes;
        //// This is the expected data type
        ExpectedType            etype;
};
}
#endif

