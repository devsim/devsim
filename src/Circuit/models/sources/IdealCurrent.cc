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

void IdealCurrent::assembleDC_impl(const NodeKeeper::Solution &sol, dsMath::RealRowColValueVec<double> &vec, dsMath::RHSEntryVec<double> &rhs)
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

