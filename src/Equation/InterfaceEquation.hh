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
typedef std::vector<RowColVal<double> > RealRowColValueVec;
typedef std::pair<int, double> RHSEntry;
typedef std::vector<RHSEntry> RHSEntryVec;
}

/// Similar to a contact, but it depends on multiple regions
class InterfaceEquation {
    public:
        /// The first region is the master region.  It shouldn't be permuted out
        /// Region is already owned by the Interface
        InterfaceEquation(const std::string &/*eqn*/, const std::string &/*var*/, InterfacePtr);

        virtual ~InterfaceEquation() = 0;

        void Assemble(dsMath::RealRowColValueVec &, dsMath::RHSEntryVec &, PermutationMap &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);

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
        void NodeVolumeType1Assemble(const std::string &, dsMath::RealRowColValueVec &, dsMath::RHSEntryVec &, PermutationMap &, dsMathEnum::WhatToLoad, const std::string &/*surface_area*/);

//// No permutation of flux equations.  Flux added (and subtracted) from both of original equations
        void NodeVolumeType2Assemble(const std::string &, dsMath::RealRowColValueVec &, dsMath::RHSEntryVec &, PermutationMap &, dsMathEnum::WhatToLoad, const std::string &/*surface_area*/);

        const std::string &GetVariable() const
        {
          return variable;
        }

    private:
        /// Use same permutation map and error out if contact had already swapped it out
        virtual void DerivedAssemble(dsMath::RealRowColValueVec &, dsMath::RHSEntryVec &, PermutationMap &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode) = 0;

        InterfaceEquation();
        InterfaceEquation(const InterfaceEquation &);
        InterfaceEquation &operator=(const InterfaceEquation &);

        std::string myname;
        std::string variable;
        InterfacePtr myinterface;
};
#endif
