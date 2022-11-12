/***
DEVSIM
Copyright 2013 DEVSIM LLC

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

enum class ExpectedType {UNKNOWN = 0, NODE, EDGE, TRIANGLEEDGE, TETRAHEDRONEDGE};

template <typename DoubleType>
class ModelExprEval {
    public:
        typedef std::vector<ModelExprData<DoubleType> > margv_t;
        typedef std::list<std::string> error_t;
        /// need to generalize to contact and interface equations as well
        typedef const Region * data_ref_t;
        /// we are given a reference to our callbacks
        /// could just as soon be a class which is called back for these variables
        ModelExprEval(data_ref_t &, const std::string &, error_t &);
        ~ModelExprEval();

        ModelExprData<DoubleType> eval_function(Eqo::EqObjPtr);
    private:
        ModelExprData<DoubleType> EvaluateAddType(Eqo::EqObjPtr);
        ModelExprData<DoubleType> EvaluateProductType(Eqo::EqObjPtr);
        ModelExprData<DoubleType> EvaluateVariableType(Eqo::EqObjPtr);
        ModelExprData<DoubleType> EvaluateConstantType(Eqo::EqObjPtr);
        ModelExprData<DoubleType> EvaluateModelType(Eqo::EqObjPtr);
        ModelExprData<DoubleType> EvaluateIfType(Eqo::EqObjPtr);
        ModelExprData<DoubleType> EvaluateIfElseType(Eqo::EqObjPtr);
        ModelExprData<DoubleType> EvaluateFunctionType(Eqo::EqObjPtr);
        ModelExprData<DoubleType> EvaluateInvalidType(Eqo::EqObjPtr);
        ///  Handles math functions (log, pow, exp, . . .)
        ModelExprData<DoubleType> EvaluateExternalMath(const std::string &, margv_t &);
//      ModelExprData<DoubleType> createModelExprData(Eqo::EqObjPtr);
        data_ref_t              &data_ref; // reference to data for variables
        const std::string       model;     // model being evaluated
        error_t                 &errors;
        std::vector<size_t>     indexes;
        //// This is the expected data type
        ExpectedType            etype;
};
}
#endif

