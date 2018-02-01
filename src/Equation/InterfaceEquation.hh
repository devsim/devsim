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

#ifndef INTERFACE_EQUATION_HH
#define INTERFACE_EQUATION_HH
#include "MathEnum.hh"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iosfwd>

class ObjectHolder;

class PermutationEntry;
typedef std::map<size_t, PermutationEntry> PermutationMap;

class Interface;
typedef Interface *InterfacePtr;

class Region;
typedef Region *RegionPtr;

class Node;
typedef std::vector<const Node *> ConstNodeList_t;
typedef const Node * ConstNodePtr;

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
class InterfaceEquation {
    public:
        /// The first region is the master region.  It shouldn't be permuted out
        /// Region is already owned by the Interface
        InterfaceEquation(const std::string &/*eqn*/, const std::string &/*var*/, InterfacePtr);

        virtual ~InterfaceEquation() = 0;

        void Assemble(dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, PermutationMap &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);

        const std::string &GetName() const {
            return myname;
        }

        const Interface &GetInterface() const
        {
            return *myinterface;
        }

        std::set<ConstNodePtr> GetActiveNodes() const;

        void DevsimSerialize(std::ostream &) const;
        void GetCommandOptions(std::map<std::string, ObjectHolder> &) const;

    protected:

        virtual void Serialize(std::ostream &) const = 0;
        virtual void GetCommandOptions_Impl(std::map<std::string, ObjectHolder> &) const = 0;

        ConstNodeList_t GetActiveNodesFromList(const Region &, const ConstNodeList_t &) const;

//// Permutation and additional equation
        void NodeVolumeType1Assemble(const std::string &, dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, PermutationMap &, dsMathEnum::WhatToLoad, const std::string &/*surface_area*/);

//// No permutation of flux equations.  Flux added (and subtracted) from both of original equations
        void NodeVolumeType2Assemble(const std::string &, dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, PermutationMap &, dsMathEnum::WhatToLoad, const std::string &/*surface_area*/);
//// Permutation of flux equations.  Flux subtracted from second of original equations
        void NodeVolumeType3Assemble(const std::string &, dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, PermutationMap &, dsMathEnum::WhatToLoad, const std::string &/*surface_area*/);

        const std::string &GetVariable() const
        {
          return variable;
        }

    private:
        /// Use same permutation map and error out if contact had already swapped it out
        virtual void DerivedAssemble(dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, PermutationMap &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode) = 0;

        InterfaceEquation();
        InterfaceEquation(const InterfaceEquation &);
        InterfaceEquation &operator=(const InterfaceEquation &);

        std::string myname;
        std::string variable;
        InterfacePtr myinterface;
};
#endif
