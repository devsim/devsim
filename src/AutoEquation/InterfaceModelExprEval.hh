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
class InterfaceModelExprEval {
    public:
        typedef std::vector<InterfaceModelExprData> margv_t;
        typedef std::list<std::string> error_t;
        /// need to generalize to contact and interface equations as well
        typedef const Interface * data_ref_t;
        /// we are given a reference to our callbacks
        /// could just as soon be a class which is called back for these variables
        InterfaceModelExprEval(data_ref_t &, error_t &);
//      InterfaceModelExprEval(data_ref_t &, error_t &, double); // for creation from double
        ~InterfaceModelExprEval();

        InterfaceModelExprData eval_function(Eqo::EqObjPtr);

    private:
        InterfaceModelExprData EvaluateAddType(Eqo::EqObjPtr);
        InterfaceModelExprData EvaluateProductType(Eqo::EqObjPtr);
        InterfaceModelExprData EvaluateVariableType(Eqo::EqObjPtr);
        InterfaceModelExprData EvaluateConstantType(Eqo::EqObjPtr);
        InterfaceModelExprData EvaluateModelType(Eqo::EqObjPtr);
        InterfaceModelExprData EvaluateIfType(Eqo::EqObjPtr);
        InterfaceModelExprData EvaluateIfElseType(Eqo::EqObjPtr);
        InterfaceModelExprData EvaluateFunctionType(Eqo::EqObjPtr);
        InterfaceModelExprData EvaluateInvalidType(Eqo::EqObjPtr);

        void GetRegionAndName(const std::string &nm, std::string &name, const Region *&r);
        InterfaceModelExprData EvaluateInterfaceModelType(Eqo::EqObjPtr);
        ///  Handles math functions (log, pow, exp, . . .)
        InterfaceModelExprData EvaluateExternalMath(const std::string &, const margv_t &);
//      InterfaceModelExprData createInterfaceModelExprData(Eqo::EqObjPtr);
        data_ref_t &data_ref; // reference to data for variables
        error_t &errors;
};
}
#endif

