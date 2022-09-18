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
    typedef std::shared_ptr<EquationObject> EqObjPtr;

}
*/

class PermutationEntry;

template <typename DoubleType>
class ExprEquation : public Equation<DoubleType>
{
    public:
        ExprEquation(const std::string &, /*Equation Name we are giving ourself*/
                     RegionPtr,           /*Region we are being solved in*/
                     const std::string &, /*variable Name being solved*/
                     const std::string &, /*NodeModel Name for integrating source/sink*/
                     const std::string &, /*EdgeModel Name for integrating flux*/
                     const std::string &, /*EdgeModel for volumes integration over edge */
                     const std::string &, /*ElementEdgeModel Name for integrating flux*/
                     const std::string &, /*ElementVolumeModel Name for integrating source (node 0)*/
                     const std::string &, /*ElementVolumeModel Name for integrating source (node 1)*/
                     const std::string &, /*Time-dependent NodeModel Name for integrating source/sink*/
                        //// Some day we may have edge time dependent model
                     typename EquationEnum::UpdateType/* = Equation::DEFAULT*/
                        );


    private:
        void Serialize(std::ostream &) const;
        void GetCommandOptions_Impl(std::map<std::string, ObjectHolder> &) const;

        ExprEquation();
        ExprEquation(const ExprEquation &);
        ExprEquation &operator=(const ExprEquation &);

        void DerivedAssemble(dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);

        void UpdateValues(NodeModel &, const std::vector<DoubleType> &);
        void ACUpdateValues(NodeModel &, const std::vector<std::complex<DoubleType> > &);
        void NoiseUpdateValues(const std::string &, const std::vector<PermutationEntry> &, const std::vector<std::complex<DoubleType> > &);

        /// Need to decide if we are going to contain equations or models?
        /// Assume that
        std::string node_model_;
        std::string edge_model_;
        std::string edge_volume_model_;
        std::string element_model_;
        std::string volume_node0_model_;
        std::string volume_node1_model_;
        std::string time_node_model_;
};
#endif
