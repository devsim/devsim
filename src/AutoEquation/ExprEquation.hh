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

#ifndef EXPREQUATION_HH
#define EXPREQUATION_HH
#include "Equation.hh"
#include "MathEnum.hh"

#include <complex>
#include <string>

/*
#include <memory>
namespace Eqo {
    class EquationObject;
    typedef std::tr1::shared_ptr<EquationObject> EqObjPtr;

}
*/


class ExprEquation : public Equation
{
    public:
        ExprEquation(const std::string &, /*Equation Name we are giving ourself*/
                     RegionPtr,           /*Region we are being solved in*/
                     const std::string &, /*variable Name being solved*/
                     const std::string &, /*NodeModel Name for integrating source/sink*/
                     const std::string &, /*EdgeModel Name for integrating flux*/
                     const std::string &, /*ElementEdgeModel Name for integrating flux*/
                     const std::string &, /*ElementVolumeModel Name for integrating source*/
                     const std::string &, /*Time-dependent NodeModel Name for integrating source/sink*/
                        //// Some day we may have edge time dependent model
                     Equation::UpdateType/* = Equation::DEFAULT*/
                        );

            void Serialize(std::ostream &) const;

    private:
        ExprEquation();
        ExprEquation(const ExprEquation &);
        ExprEquation &operator=(const ExprEquation &);

        void DerivedAssemble(dsMath::RealRowColValueVec &, dsMath::RHSEntryVec &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);

        void UpdateValues(NodeModel &, const std::vector<double> &);
        void ACUpdateValues(NodeModel &, const std::vector<std::complex<double> > &);
        void NoiseUpdateValues(const std::string &, const std::vector<size_t> &, const std::vector<std::complex<double> > &);

        /// Need to decide if we are going to contain equations or models?
        /// Assume that 
        std::string node_model_;
        std::string edge_model_;
        std::string element_model_;
        std::string volume_model_;
        std::string time_node_model_;
};
#endif
