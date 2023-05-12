/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef IDEAL_VOLTAGE_HH
#define IDEAL_VOLTAGE_HH
#include "InstanceModel.hh"
#include "CircuitNode.hh"
#include "Signal.hh"
#include "MatrixEntries.hh"

#include <string>

// Needs to be derived from InstanceModel
class IdealVoltage : public InstanceModel {
    public:
        IdealVoltage(
                NodeKeeper *,
                const char *name, // level name included
                const char *n1,
                const char *n2,
                double value,
                const SignalPtr
//              const ModelPtr,
//              const InstanceParameterList &
                );

        IdealVoltage(
                NodeKeeper *,
                const char *name, // level name included
                const char *n1,
                const char *n2,
                double value,
                double acreal,
                double acimag,
                const SignalPtr
//              const ModelPtr,
//              const InstanceParameterList &
                );

//      IdealVoltage(const char *nplus, const char *nmin, const char *value);
//      void getDCStamp(Matrix::RowColEntryVec &);
//      void getTranStamp(Matrix::RowColEntryVec &) {};
        bool addParam(const std::string &, double);
    private:
        void assembleACRHS_impl(std::vector<std::pair<size_t, std::complex<double> > > &);
        void assembleDC_impl(const NodeKeeper::Solution &, dsMath::RealRowColValueVec<double> &, dsMath::RHSEntryVec<double> &);
        void assembleTran_impl(const double scl, const NodeKeeper::Solution &sol, dsMath::RealRowColValueVec<double> *mat, dsMath::RHSEntryVec<double> &rhs) {};

        IdealVoltage();
        IdealVoltage(const IdealVoltage &);
        IdealVoltage operator=(const IdealVoltage &);

        double  v_; // DC Voltage
        double  acrv_; // AC Voltage
        double  aciv_; // AC Voltage
        CircuitNodePtr np_;
        CircuitNodePtr nm_;
        CircuitNodePtr nI_;  // Internal MNA Node
        SignalPtr sig_; // My signal which controls everything
};

#endif
