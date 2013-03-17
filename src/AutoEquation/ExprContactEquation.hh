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

#ifndef EXPR_CONTACT_EQUATION_HH
#define EXPR_CONTACT_EQUATION_HH
#include "ContactEquation.hh"
#include "Permutation.hh"
// Default ideal equation
class ExprContactEquation : public ContactEquation
{
    public:
        /// eventually, we will need to handle time dependent case
        ExprContactEquation(
            const std::string &eq,// equation
            const std::string &var, // variable
            ContactPtr,
            RegionPtr,
            const std::string &,// nodemodel
            const std::string &,// edgemodel
            const std::string &,// elementedgemodel
            const std::string &,// nodemodel_current
            const std::string &,// edgemodel_current
            const std::string &,// elementedgemodel_current
            const std::string &,// nodemodel_charge
            const std::string &,// edgemodel_charge
            const std::string &// elementedgemodel_charge
            );

            void Serialize(std::ostream &) const;

    private:
        void calcCurrent();
        void calcCharge();

        void DerivedAssemble(dsMath::RealRowColValueVec &, RHSEntryVec &, PermutationMap &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);


        ExprContactEquation();
        ExprContactEquation(const ExprContactEquation &);
        ExprContactEquation &operator=(const ExprContactEquation &);

        /// If both of the following strings are empty, we do not permute out the original
        /// ones.  Note these models have been calculated over the whole bulk.
        const std::string nodemodel_int; /// model replacing nodal integration
        const std::string edgemodel_int; /// model replacing flux integration
        const std::string elementedgemodel_int; /// model replacing flux integration
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
