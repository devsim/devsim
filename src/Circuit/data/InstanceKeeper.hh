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
