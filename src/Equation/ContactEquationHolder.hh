/***
DEVSIM
Copyright 2017 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef CONTACTEQUATIONHOLDER_HH
#define CONTACTEQUATIONHOLDER_HH

#include "MathEnum.hh"

#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
#endif

class ObjectHolder;

#include <string>
#include <iosfwd>
#include <vector>
#include <complex>
#include <map>
#include <memory>

template <typename DoubleType>
class ContactEquation;

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

class ContactEquationHolder {
  public:
    ContactEquationHolder() {}

    template <typename DoubleType>
    ContactEquationHolder(ContactEquation<DoubleType> *);

    ~ContactEquationHolder();

    template <typename DoubleType>
    DoubleType GetCurrent() const;

    template <typename DoubleType>
    DoubleType GetCharge() const;

    std::string GetName() const;
    bool operator==(const ContactEquationHolder &) const;
    void UpdateContact() const;
    template <typename DoubleType>
    void Assemble(dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, PermutationMap &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);
    void DevsimSerialize(std::ostream &) const;
    void GetCommandOptions(std::map<std::string, ObjectHolder> &) const;
  private:
    std::shared_ptr<ContactEquation<double>> double_;
#ifdef DEVSIM_EXTENDED_PRECISION
    std::shared_ptr<ContactEquation<float128>> float128_;
#endif
};
#endif

