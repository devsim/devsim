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

#ifndef INTERFACEEXPREQUATION_HH
#define INTERFACEEXPREQUATION_HH
#include "InterfaceEquation.hh"

#include <string>

/*
#include <memory>
namespace Eqo {
    class EquationObject;
    typedef std::tr1::shared_ptr<EquationObject> EqObjPtr;

}
*/


/**
 * This type of equation permutes the second regions equation into the first.
 */
class InterfaceExprEquation : public InterfaceEquation
{
    public:
        enum EquationType {UNKNOWN, CONTINUOUS, FLUXTERM};

        static const char *EquationTypeString[];

        InterfaceExprEquation(const std::string &, /*Equation Name we are giving ourself*/
                     InterfacePtr,           /*Region we are being solved in*/
                     const std::string &, /*variable Name being solved*/
                     const std::string &, /*InterfaceNodeModel Name for region*/
                     EquationType
                        );
            void Serialize(std::ostream &) const;

    private:
        InterfaceExprEquation();
        InterfaceExprEquation(const InterfaceExprEquation &);
        InterfaceExprEquation &operator=(const InterfaceExprEquation &);

        void DerivedAssemble(dsMath::RealRowColValueVec &, dsMath::RHSEntryVec &, PermutationMap &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);

        /// Need to decide if we are going to contain equations or models?
        /// Assume that 
        std::string   interface_node_model_;
        EquationType  equation_type_;
        //std::string tdnodemodel; // maybe someday in the future
};
#endif
