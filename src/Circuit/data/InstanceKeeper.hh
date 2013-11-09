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
typedef std::vector<RowColVal<double> > RealRowColValueVec;
typedef std::pair<int, double> RHSEntry;
typedef std::vector<RHSEntry> RHSEntryVec;
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

        void AssembleDCMatrix(dsMath::RealRowColValueVec &, const NodeKeeper::Solution &sol, dsMath::RHSEntryVec &rhs);
        //// This is also used for the ssac
        void AssembleTRMatrix(dsMath::RealRowColValueVec *, const NodeKeeper::Solution &sol, dsMath::RHSEntryVec &rhs, double scl);
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
