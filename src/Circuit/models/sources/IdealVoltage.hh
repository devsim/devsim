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
        void assembleDC(const NodeKeeper::Solution &, dsMath::RealRowColValueVec &, dsMath::RHSEntryVec &);
        void assembleACRHS(std::vector<std::pair<size_t, std::complex<double> > > &);
//      void getTranStamp(Matrix::RowColEntryVec &) {};
        void assembleTran(const double scl, const NodeKeeper::Solution &sol, dsMath::RealRowColValueVec *mat, dsMath::RHSEntryVec &rhs) {};
        bool addParam(const std::string &, double);
    private:
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
