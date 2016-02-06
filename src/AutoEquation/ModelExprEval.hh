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

