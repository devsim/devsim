/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef INTERFACEMODELEXPREVAL_HH
#define INTERFACEMODELEXPREVAL_HH

#include <memory>

// use a map to look up values in our class
// maybe turn this class into a template for scalars and vectors later on
#include <map>
#include <string>
#include <list>
#include <vector>
class Region;
typedef Region *RegionPtr;

class Interface;

namespace Eqo {
class EquationObject;
typedef std::shared_ptr<EquationObject> EqObjPtr;
}

#include "InterfaceModelExprData.hh"

namespace IMEE {
template <typename DoubleType>
class InterfaceModelExprEval {
    public:
        typedef std::vector<InterfaceModelExprData<DoubleType> > margv_t;
        typedef std::list<std::string> error_t;
        /// need to generalize to contact and interface equations as well
        typedef const Interface * data_ref_t;
        /// we are given a reference to our callbacks
        /// could just as soon be a class which is called back for these variables
        InterfaceModelExprEval(data_ref_t &, error_t &);
        ~InterfaceModelExprEval();

        InterfaceModelExprData<DoubleType> eval_function(Eqo::EqObjPtr);

    private:
        InterfaceModelExprData<DoubleType> EvaluateAddType(Eqo::EqObjPtr);
        InterfaceModelExprData<DoubleType> EvaluateProductType(Eqo::EqObjPtr);
        InterfaceModelExprData<DoubleType> EvaluateVariableType(Eqo::EqObjPtr);
        InterfaceModelExprData<DoubleType> EvaluateConstantType(Eqo::EqObjPtr);
        InterfaceModelExprData<DoubleType> EvaluateIfType(Eqo::EqObjPtr);
        InterfaceModelExprData<DoubleType> EvaluateIfElseType(Eqo::EqObjPtr);
        InterfaceModelExprData<DoubleType> EvaluateFunctionType(Eqo::EqObjPtr);
        InterfaceModelExprData<DoubleType> EvaluateInvalidType(Eqo::EqObjPtr);

        void GetRegionAndName(const std::string &nm, std::string &name, const Region *&r);
        InterfaceModelExprData<DoubleType> EvaluateInterfaceModelType(Eqo::EqObjPtr);
        ///  Handles math functions (log, pow, exp, . . .)
        InterfaceModelExprData<DoubleType> EvaluateExternalMath(const std::string &, const margv_t &);
//      InterfaceModelExprData<DoubleType> createInterfaceModelExprData(Eqo::EqObjPtr);
        data_ref_t &data_ref; // reference to data for variables
        error_t &errors;
};
}
#endif

