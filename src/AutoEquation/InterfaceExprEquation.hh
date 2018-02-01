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

#ifndef INTERFACEEXPREQUATION_HH
#define INTERFACEEXPREQUATION_HH
#include "InterfaceEquation.hh"

#include <string>

/*
#include <memory>
namespace Eqo {
    class EquationObject;
    typedef std::shared_ptr<EquationObject> EqObjPtr;

}
*/


/**
 * This type of equation permutes the second regions equation into the first.
 */
namespace InterfaceExprEquationEnum
{
  enum EquationType {UNKNOWN, CONTINUOUS, FLUXTERM, HYBRID};
  extern const char *EquationTypeString[];
}
template <typename DoubleType>
class InterfaceExprEquation : public InterfaceEquation<DoubleType>
{
    public:


        InterfaceExprEquation(const std::string &, /*Equation Name we are giving ourself*/
                     InterfacePtr,           /*Region we are being solved in*/
                     const std::string &, /*variable Name being solved*/
                     const std::string &, /*InterfaceNodeModel Name for region*/
                     InterfaceExprEquationEnum::EquationType
                        );

    private:
        void Serialize(std::ostream &) const;
        void GetCommandOptions_Impl(std::map<std::string, ObjectHolder> &) const;

        InterfaceExprEquation();
        InterfaceExprEquation(const InterfaceExprEquation &);
        InterfaceExprEquation &operator=(const InterfaceExprEquation &);

        void DerivedAssemble(dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, PermutationMap &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);

        /// Need to decide if we are going to contain equations or models?
        /// Assume that 
        std::string   interface_node_model_;
        InterfaceExprEquationEnum::EquationType  equation_type_;
        //std::string tdnodemodel; // maybe someday in the future
};
#endif

