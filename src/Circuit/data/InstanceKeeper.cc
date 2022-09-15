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

#include "InstanceKeeper.hh"
#include "InstanceModel.hh"
//#include "matrix.hh"
#include "Signal.hh"
#include "dsAssert.hh"

#include <cmath>
#include <complex>
#include <iostream>
#include <utility>
#include <vector>

using std::complex;
using std::pair;
using std::vector;

// This class should not ever know the particulars of any one model type

InstanceKeeper *InstanceKeeper::instance_ = 0;

InstanceKeeper &InstanceKeeper::instance() {
  if (!instance_) {
    instance_ = new InstanceKeeper;
  }
  return *instance_;
}

void InstanceKeeper::delete_instance() { delete instance_; }

InstanceKeeper::InstanceKeeper() {}

InstanceModelPtr InstanceKeeper::addInstanceModel(InstanceModelPtr p) {
  const std::string nm = p->getName();
  //    os << "adding model " << nm << "\n";

  dsAssert(instMod_.count(nm) == 0, "CIRCUIT_UNEXPECTED");

  if (instMod_.count(nm) == 0)
    instMod_[nm] = p;

  return instMod_[nm];
}

InstanceModelPtr InstanceKeeper::addInstanceModel(InstanceModel *p) {
  InstanceModelPtr t1(p);
  return addInstanceModel(t1);
}

// This class will go through each model and add them to
// their elements to the matrix
// This would be a bottleneck for parallel processing since
// everyone is trying to assemble to the same matrix.  Perhaps each
// each model thread should return a vector of row,col,val which can
// then be added to the matrix
//
// precomputation iterator can be handled later
void InstanceKeeper::AssembleDCMatrix(dsMath::RealRowColValueVec<double> &mat,
                                      const std::vector<double> &sol,
                                      dsMath::RHSEntryVec<double> &rhs) {
  InstanceModelList::iterator iter, end = instMod_.end();

  for (iter = instMod_.begin(); iter != end; ++iter) {
    iter->second->assembleDC(sol, mat, rhs);
  }
}

// The next two are the AC terms.  Make sure they look the same as above.
void InstanceKeeper::AssembleTRMatrix(dsMath::RealRowColValueVec<double> *mat,
                                      const std::vector<double> &sol,
                                      dsMath::RHSEntryVec<double> &rhs,
                                      double scl) {
  InstanceModelList::iterator iter, end = instMod_.end();
  for (iter = instMod_.begin(); iter != end; ++iter) {
    // os << "assembling " << iter->first << "\n";
    //// Get this working first, but do we need to load the rhs?? for
    /// small-signal AC
    iter->second->assembleTran(scl, sol, mat, rhs);
  }
}

void InstanceKeeper::assembleACRHS(
    std::vector<std::pair<size_t, std::complex<double>>> &rhs) {
  InstanceModelList::iterator iter, end = instMod_.end();
  for (iter = instMod_.begin(); iter != end; ++iter) {
    iter->second->assembleACRHS(rhs);
  }
}

void InstanceKeeper::addSignal(SignalPtr x) { sigList_.push_back(x); }

void InstanceKeeper::updateSignals(double t) {
  SignalList::iterator iter, end = sigList_.end();
  for (iter = sigList_.begin(); iter != end; ++iter) {
    (*iter)->CalcVoltage(t);
  }
}

InstanceModelPtr InstanceKeeper::getInstanceModel(const std::string &name) {
  InstanceModelPtr ret;

  if (instMod_.count(name)) {
    ret = instMod_[name];
  }

  return ret;
}
