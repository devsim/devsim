/***
DEVSIM
Copyright 2017 Devsim LLC

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

#ifndef EQUATIONHOLDER_HH
#define EQUATIONHOLDER_HH

#include "MathEnum.hh"

class ObjectHolder;

template <typename DoubleType>
class Equation;

class NodeModel;

#include <string>
#include <iosfwd>
#include <vector>
#include <complex>
#include <map>
#include <memory>

namespace dsMath {
template <typename T> class RowColVal;

template <typename DoubleType>
using RealRowColVal = RowColVal<DoubleType>;

template <typename DoubleType>
using RealRowColValueVec = std::vector<RealRowColVal<DoubleType>>;

template <typename DoubleType>
using RHSEntry = std::pair<int, DoubleType>;

template <typename DoubleType>
using RHSEntryVec = std::vector<RHSEntry<DoubleType>>;
}

class EquationHolder {
  public:
    EquationHolder() : double_(nullptr) {}
    EquationHolder(Equation<double> *);

    std::string GetName() const;
    std::string GetVariable() const;
    bool operator==(const EquationHolder &) const;
    void DevsimSerialize(std::ostream &) const;

    double GetRelError() const;
    double GetAbsError() const;
    void Update(NodeModel &, const std::vector<double> &) const;
    void ACUpdate(NodeModel &, const std::vector<std::complex<double>> &) const;
    void NoiseUpdate(const std::string &, const std::vector<size_t> &, const std::vector<std::complex<double> > &) const;
    void Assemble(dsMath::RealRowColValueVec<double> &, dsMath::RHSEntryVec<double> &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);
    void GetCommandOptions(std::map<std::string, ObjectHolder> &) const;

    ~EquationHolder();
  private:
    std::shared_ptr<Equation<double>> double_;
};
#endif

