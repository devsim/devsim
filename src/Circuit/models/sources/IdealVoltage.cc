/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "IdealVoltage.hh"
#include "Signal.hh"
#include "CircuitNode.hh"
#include "InstanceKeeper.hh"

#include <string>
#include <complex>

IdealVoltage::IdealVoltage(
                NodeKeeper *nkp,
                const char *name, // level name included
                const char *n1,
                const char *n2,
                double     value, // DV voltage
                const SignalPtr sig
                ) : InstanceModel(nkp, name), acrv_(0.0), aciv_(0.0)
{
    sig_ = sig;
    InstanceKeeper::instance().addSignal(sig);
    np_ = this->AddCircuitNode(n1);
    nm_ = this->AddCircuitNode(n2);

    nI_ = this->AddMNANode("I");

    v_ = value;
}

IdealVoltage::IdealVoltage(
                NodeKeeper *nkp,
                const char *name, // level name included
                const char *n1,
                const char *n2,
                double     value, // DV voltage
                double     acrvalue,
                double     acivalue,
                const SignalPtr sig
                ) : InstanceModel(nkp, name)
{
    sig_ = sig;
    np_ = this->AddCircuitNode(n1);
    nm_ = this->AddCircuitNode(n2);

    nI_ = this->AddMNANode("I");

    v_ = value;
    acrv_ = acrvalue;
    aciv_ = acivalue;
}

#if 0
// MNA stamp for an ideal voltage source
void IdealVoltage::getDCStamp(Matrix::RowColEntryVec &vec)
{
    vec.reserve(4);
    const int rp = np_->getNumber();
    const int rm = nm_->getNumber();
    const int rI = nI_->getNumber();

    if (!np_->isGROUND())
    {
        vec.push_back(Matrix::RowColEntry(rp, rI));
        vec.push_back(Matrix::RowColEntry(rI, rp));
    }
    if (!nm_->isGROUND())
    {
        vec.push_back(Matrix::RowColEntry(rm, rI));
        vec.push_back(Matrix::RowColEntry(rI, rm));
    }
}
#endif

// I should separate out rhs assembly from dc assembly
void IdealVoltage::assembleDC_impl(const NodeKeeper::Solution &sol, dsMath::RealRowColValueVec<double> &vec, dsMath::RHSEntryVec<double> &rhs)
{
    v_ = sig_->getVoltage();
    const double G = 1.0;

    const int rp = np_->getNumber();
    const int rm = nm_->getNumber();
    const int rI = nI_->getNumber();

    rhs.reserve(3);
    double vp = 0.0;
    if (!np_->isGROUND())
        vp = sol[rp];

    double vm = 0.0;
    if (!nm_->isGROUND())
        vm = sol[rm];

    const double V = -v_ + vp - vm;
    const double I = sol[rI];

    vec.reserve(4);

    if (!np_->isGROUND())
    {
        vec.push_back(dsMath::RealRowColVal<double>(rp, rI, G));
        vec.push_back(dsMath::RealRowColVal<double>(rI, rp, G));
        rhs.push_back(std::make_pair(rp,I));
    }
    if (!nm_->isGROUND())
    {
        vec.push_back(dsMath::RealRowColVal<double>(rm, rI, -G));
        vec.push_back(dsMath::RealRowColVal<double>(rI, rm, -G));
        rhs.push_back(std::make_pair(rm,-I));
    }
    rhs.push_back(std::make_pair(rI,V));
}

#if 1
#include <iostream>
//using namespace std;
// assume for now you can only have one value
void IdealVoltage::assembleACRHS_impl(std::vector<std::pair<size_t, std::complex<double> > > &x) {
    const int rI = nI_->getNumber();
    x.push_back(std::make_pair(rI, std::complex<double>(acrv_,aciv_)));
/*
    for (size_t i = 0; i < x.size(); ++i)
    {
        cout << x[i].first << " " << x[i].second << endl;
    }
*/
};
#endif

bool IdealVoltage::addParam(const std::string &nm, double v)
{
    bool ret = false;
    if (nm == "V")
    {
        sig_->setVoltage(v);
        ret = true;
    }
    else if (nm == "acreal")
    {
        acrv_ = v;
        ret = true;
    }
    else if (nm == "acimag")
    {
        aciv_ = v;
        ret = true;
    }
    return ret;
}

