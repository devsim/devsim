/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef IDEAL_CURRENT_HH
#define IDEAL_CURRENT_HH
#include "InstanceModel.hh"
#include "CircuitNode.hh"
#include "MatrixEntries.hh"
#include "Signal.hh"

#include <string>

// Needs to be derived from InstanceModel
class IdealCurrent : public InstanceModel {
    public:
        IdealCurrent(
                NodeKeeper *,
                const char *name, // level name included
                const char *n1,
                const char *n2,
                double value,
                const SignalPtr
//              const ModelPtr,
//              const InstanceParameterList &
                );

//      IdealCurrent(const char *nplus, const char *nmin, const char *value);

//      void getDCStamp(Matrix::RowColEntryVec &);
//      void getTranStamp(Matrix::RowColEntryVec &) {};
        bool addParam(const std::string &, double);

    private:
        void assembleDC_impl(const NodeKeeper::Solution &, dsMath::RealRowColValueVec<double> &, dsMath::RHSEntryVec<double> &);
        void assembleTran_impl(const double scl, const NodeKeeper::Solution &sol, dsMath::RealRowColValueVec<double> *mat, std::vector<std::pair<int, double> > &rhs) {};

        IdealCurrent();
        IdealCurrent(const IdealCurrent &);
        IdealCurrent operator=(const IdealCurrent &);

        double  i_; // DC current
        CircuitNodePtr np_;
        CircuitNodePtr nm_;
        SignalPtr sig_; // My signal which controls everything
};

#endif
