/***
DEVSIM
Copyright 2017 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef EQUATIONHOLDER_HH
#define EQUATIONHOLDER_HH

#include "MathEnum.hh"

#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
#endif
#include "dsMathTypes.hh"

class PermutationEntry;
class ObjectHolder;

template <typename DoubleType>
class Equation;

class NodeModel;

#include <string>
#include <iosfwd>
#include <vector>
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
    EquationHolder() {}

    template <typename DoubleType>
    EquationHolder(Equation<DoubleType> *);

    std::string GetName() const;
    std::string GetVariable() const;
    bool operator==(const EquationHolder &) const;
    void DevsimSerialize(std::ostream &) const;

    template <typename DoubleType>
    DoubleType GetRelError() const;

    template <typename DoubleType>
    DoubleType GetAbsError() const;

    size_t GetRelErrorNodeIndex() const;

    size_t GetAbsErrorNodeIndex() const;

    template <typename DoubleType>
    void Update(NodeModel &, const dsMath::DoubleVec_t<DoubleType> &) const;

    template <typename DoubleType>
    void ACUpdate(NodeModel &, const dsMath::ComplexDoubleVec_t<DoubleType> &) const;

    template <typename DoubleType>
    void NoiseUpdate(const std::string &, const std::vector<PermutationEntry> &, const dsMath::ComplexDoubleVec_t<DoubleType> &) const;

    template <typename DoubleType>
    void Assemble(dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);

    void GetCommandOptions(std::map<std::string, ObjectHolder> &) const;

    ~EquationHolder();
  private:
    std::shared_ptr<Equation<double>> double_;
#ifdef DEVSIM_EXTENDED_PRECISION
    std::shared_ptr<Equation<float128>> float128_;
#endif
};
#endif

