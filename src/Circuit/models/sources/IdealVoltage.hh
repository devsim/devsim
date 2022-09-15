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

#ifndef IDEAL_VOLTAGE_HH
#define IDEAL_VOLTAGE_HH
#include "CircuitNode.hh"
#include "InstanceModel.hh"
#include "MatrixEntries.hh"
#include "Signal.hh"

#include <string>

// Needs to be derived from InstanceModel
class IdealVoltage : public InstanceModel {
public:
  IdealVoltage(NodeKeeper *,
               const char *name, // level name included
               const char *n1, const char *n2, double value, const SignalPtr
               //              const ModelPtr,
               //              const InstanceParameterList &
  );

  IdealVoltage(NodeKeeper *,
               const char *name, // level name included
               const char *n1, const char *n2, double value, double acreal,
               double acimag, const SignalPtr
               //              const ModelPtr,
               //              const InstanceParameterList &
  );

  //      IdealVoltage(const char *nplus, const char *nmin, const char *value);
  //      void getDCStamp(Matrix::RowColEntryVec &);
  //      void getTranStamp(Matrix::RowColEntryVec &) {};
  bool addParam(const std::string &, double);

private:
  void
  assembleACRHS_impl(std::vector<std::pair<size_t, std::complex<double>>> &);
  void assembleDC_impl(const NodeKeeper::Solution &,
                       dsMath::RealRowColValueVec<double> &,
                       dsMath::RHSEntryVec<double> &);
  void assembleTran_impl(const double scl, const NodeKeeper::Solution &sol,
                         dsMath::RealRowColValueVec<double> *mat,
                         dsMath::RHSEntryVec<double> &rhs){};

  IdealVoltage();
  IdealVoltage(const IdealVoltage &);
  IdealVoltage operator=(const IdealVoltage &);

  double v_;    // DC Voltage
  double acrv_; // AC Voltage
  double aciv_; // AC Voltage
  CircuitNodePtr np_;
  CircuitNodePtr nm_;
  CircuitNodePtr nI_; // Internal MNA Node
  SignalPtr sig_;     // My signal which controls everything
};

#endif
