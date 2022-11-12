/***
DEVSIM
Copyright 2017 DEVSIM LLC

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

#ifndef INTERFACEEQUATIONHOLDER_HH
#define INTERFACEEQUATIONHOLDER_HH

#include "MathEnum.hh"

#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
#endif

#include <string>
#include <iosfwd>
#include <vector>
#include <complex>
#include <map>
#include <memory>

class ObjectHolder;

class PermutationEntry;
typedef std::map<size_t, PermutationEntry> PermutationMap;

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


template <typename DoubleType>
class InterfaceEquation;
class InterfaceEquationHolder {
  public:
    InterfaceEquationHolder() {}

    template <typename DoubleType>
    InterfaceEquationHolder(InterfaceEquation<DoubleType> *);

    std::string GetName() const;
    std::string GetName0() const;
    std::string GetName1() const;
    bool operator==(const InterfaceEquationHolder &) const;

    template <typename DoubleType>
    void Assemble(dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, PermutationMap &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);

    void DevsimSerialize(std::ostream &) const;
    void GetCommandOptions(std::map<std::string, ObjectHolder> &) const;
  private:
    std::shared_ptr<InterfaceEquation<double>> double_;
#ifdef DEVSIM_EXTENDED_PRECISION
    std::shared_ptr<InterfaceEquation<float128>> float128_;
#endif
};
#endif

