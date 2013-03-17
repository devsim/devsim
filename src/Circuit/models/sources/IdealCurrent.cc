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

#include "IdealCurrent.hh"
#include "Signal.hh"
#include "CircuitNode.hh"
#include "InstanceKeeper.hh"

#include <string>

IdealCurrent::IdealCurrent(
                NodeKeeper *nkp,
                const char *name, // level name included
                const char *n1,
                const char *n2,
                double value,
                const SignalPtr sig
                ) : InstanceModel(nkp, name)
{
    sig_ = sig;
    InstanceKeeper::instance().addSignal(sig);
    np_ = this->AddCircuitNode(n1);
    nm_ = this->AddCircuitNode(n2);

    i_ = value;
}

#if 0
void IdealCurrent::getDCStamp(Matrix::RowColEntryVec &vec)
{
}
#endif

// I should separate out rhs assembly from dc assembly
void IdealCurrent::assembleDC(const NodeKeeper::Solution &sol, dsMath::RealRowColValueVec &vec, dsMath::RHSEntryVec &rhs)
{
    i_ = sig_->getVoltage();
    const int rp = np_->getNumber();
    const int rm = nm_->getNumber();

    rhs.reserve(2);

    if (!np_->isGROUND())
    {
        rhs.push_back(std::make_pair(rp,-i_));
    }
    if (!nm_->isGROUND())
    {
        rhs.push_back(std::make_pair(rm,+i_));
    }
}

bool IdealCurrent::addParam(const std::string &nm, double v)
{
    bool ret = false;
    if (nm == "I")
    {
        sig_->setVoltage(v);
        ret = true;
    }
    return ret;
}

