/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef INSTANCE_MODEL_HH
#define INSTANCE_MODEL_HH
#include "CircuitNode.hh"
#include "NodeKeeper.hh"

#include <string>

namespace dsMath {
template <typename T> class RowColVal;

template <typename DoubleType>
using RealRowColVal = RowColVal<DoubleType>;

template <typename DoubleType>
using RealRowColValueVec = std::vector<RealRowColVal<DoubleType>>;

template <typename DoubleType>
using RHSEntry = std::pair<int, DoubleType>;

template <typename DoubleType>
using RHSEntryVec = std::vector<RHSEntry<DoubleType>>;
}

class InstanceModel
{
    public:
        virtual ~InstanceModel() = 0;
        const std::string &getName() {return name_;}

        void assembleDC(const std::vector<double> &sol, dsMath::RealRowColValueVec<double> &mat, dsMath::RHSEntryVec<double> &rhs);

        // Transient stamp is only the AC part
        // scl is the scaling factor to multiply by the time-dependent elements
        // If the matrix is a nullptr pointer, only the rhs is calculated
        // This is so that it can be used for the charge contributions of
        // previous time steps
        void assembleTran(const double scl, const std::vector<double> &sol, dsMath::RealRowColValueVec<double> *mat, dsMath::RHSEntryVec<double> &rhs);

        virtual bool addParam(const std::string &, double) = 0;

        void assembleACRHS(std::vector<std::pair<size_t, std::complex<double> > > &);

    protected:
        virtual void assembleDC_impl(const std::vector<double> &sol, dsMath::RealRowColValueVec<double> &mat, dsMath::RHSEntryVec<double> &rhs)=0;
        virtual void assembleTran_impl(const double scl, const std::vector<double> &sol, dsMath::RealRowColValueVec<double> *mat, dsMath::RHSEntryVec<double> &rhs) = 0;
        virtual void assembleACRHS_impl(std::vector<std::pair<size_t, std::complex<double> > > &) {}

        // Only derived classes can instantiate the base class
        InstanceModel(NodeKeeper *, const char *name);
        CircuitNodePtr AddCircuitNode(const char *);
        CircuitNodePtr AddInternalNode(const char *);
        CircuitNodePtr AddMNANode(const char *);
    private:
        InstanceModel();
        InstanceModel(const InstanceModel &);
//      InstanceModel operator=(const InstanceModel &);

        NodeKeeper *nodekeep_;
        std::string name_;
        // Needs to query the model keeper
        // and find the appropriate Model,
        // if it has already been defined.
        //
        // Getter's and Setter's are provided
        // so that the derived class is able to
        // get the model info it needs
        // ModelPtr
};

#endif /* INSTANCE_MODEL_HH */

