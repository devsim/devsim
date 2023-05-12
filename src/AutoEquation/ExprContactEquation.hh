/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef EXPR_CONTACT_EQUATION_HH
#define EXPR_CONTACT_EQUATION_HH
#include "ContactEquation.hh"
#include "Permutation.hh"
// Default ideal equation
template <typename DoubleType>
class ExprContactEquation : public ContactEquation<DoubleType>
{
    public:
        /// eventually, we will need to handle time dependent case
        ExprContactEquation(
            const std::string &eq,// equation
            ContactPtr,
            RegionPtr,
            const std::string &,// nodemodel
            const std::string &,// edgemodel
            const std::string &,// edgevolumemodel
            const std::string &,// elementedgemodel
            const std::string &,// volumemodel
            const std::string &,// volumemodel
            const std::string &,// nodemodel_current
            const std::string &,// edgemodel_current
            const std::string &,// elementedgemodel_current
            const std::string &,// nodemodel_charge
            const std::string &,// edgemodel_charge
            const std::string &// elementedgemodel_charge
            );


    private:
        void Serialize(std::ostream &) const;
        void GetCommandOptions_Impl(std::map<std::string, ObjectHolder> &) const;

        void calcCurrent();
        void calcCharge();

        void DerivedAssemble(dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, PermutationMap &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);


        ExprContactEquation();
        ExprContactEquation(const ExprContactEquation &);
        ExprContactEquation &operator=(const ExprContactEquation &);

        /// If both of the following strings are empty, we do not permute out the original
        /// ones.  Note these models have been calculated over the whole bulk.
        const std::string nodemodel_int; /// model replacing nodal integration
        const std::string edgemodel_int; /// model replacing flux integration
        const std::string edgevolumemodel_int;
        const std::string elementedgemodel_int; /// model replacing flux integration
        const std::string volume_node0_model_int_;
        const std::string volume_node1_model_int_;
        //// For the time being assume that we don't have an internal time displacement term
        //// This may be important for different boundary conditions.
        const std::string nodemodel_current; /// model being used for external net flux
        const std::string edgemodel_current; /// model being used for external net flux
        const std::string elementedgemodel_current; /// model being used for external net flux
        const std::string nodemodel_charge; /// model being used for external net charge
        const std::string edgemodel_charge; /// model being used for external net charge
        const std::string elementedgemodel_charge; /// model being used for external net charge
        /// We assume, that the original equations will be coupled out and used for fully coupled simulation.
        /// In addition, they will be used for calculating the charge and current
};
#endif
