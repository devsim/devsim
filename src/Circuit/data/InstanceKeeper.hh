/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef INSTANCEKEEPER_HH
#define INSTANCEKEEPER_HH

#include <vector>
#include <string>
#include <utility>
#include <map>
#include <list>
#include <complex>
#include <vector>
#include <memory>

#include "InstanceModel.hh"

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


typedef std::shared_ptr<InstanceModel> InstanceModelPtr;
class Signal;
typedef std::shared_ptr<Signal> SignalPtr;

/**
 */
class InstanceKeeper {
        //something more efficient for later
        typedef std::map<std::string, InstanceModelPtr> InstanceModelList;
        typedef std::list<SignalPtr> SignalList;

    public:
        static InstanceKeeper &instance();
        static void delete_instance();

        InstanceModelPtr addInstanceModel(InstanceModelPtr);
        InstanceModelPtr addInstanceModel(InstanceModel * );

        InstanceModelPtr getInstanceModel(const std::string &);

        void addSignal(SignalPtr);
        // apply the current time step to the signal
        void updateSignals(double);

        void AssembleDCMatrix(dsMath::RealRowColValueVec<double> &, const std::vector<double> &sol, dsMath::RHSEntryVec<double> &rhs);
        //// This is also used for the ssac
        void AssembleTRMatrix(dsMath::RealRowColValueVec<double> *, const std::vector<double> &sol, dsMath::RHSEntryVec<double> &rhs, double scl);
        void assembleACRHS(std::vector<std::pair<size_t, std::complex<double> > > &rhs);



        // Some day provide convenience functions to sort by type
        // e.g. Linear, small-signal only, etc.
    private:
        InstanceKeeper();
        InstanceKeeper(const InstanceKeeper &);
        InstanceKeeper operator=(const InstanceKeeper &);

        static InstanceKeeper *instance_;

        InstanceModelList       instMod_;
        SignalList              sigList_;
};
#endif

