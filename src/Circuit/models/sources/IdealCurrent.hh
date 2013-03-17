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
        void assembleDC(const NodeKeeper::Solution &, dsMath::RealRowColValueVec &, dsMath::RHSEntryVec &);
//      void getTranStamp(Matrix::RowColEntryVec &) {};
        void assembleTran(const double scl, const NodeKeeper::Solution &sol, dsMath::RealRowColValueVec *mat, std::vector<std::pair<int, double> > &rhs) {};
        bool addParam(const std::string &, double);

    private:
        IdealCurrent();
        IdealCurrent(const IdealCurrent &);
        IdealCurrent operator=(const IdealCurrent &);

        double  i_; // DC current
        CircuitNodePtr np_;
        CircuitNodePtr nm_;
        SignalPtr sig_; // My signal which controls everything
};

#endif
