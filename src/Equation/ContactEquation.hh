/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef CONTACT_EQUATION_HH
#define CONTACT_EQUATION_HH
#include "MathEnum.hh"
#include <string>
#include <vector>
#include <map>
#include <iosfwd>

class ObjectHolder;

class PermutationEntry;
typedef std::map<size_t, PermutationEntry> PermutationMap;
class Contact;
typedef Contact *ContactPtr;

class Region;
typedef Region *RegionPtr;

class Node;
typedef std::vector<const Node *> ConstNodeList_t;

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
class ContactEquation {
    public:
        ContactEquation(const std::string &/*eqn*/, ContactPtr, RegionPtr);
        virtual ~ContactEquation() = 0;
        void Assemble(dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, PermutationMap &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);
        const std::string &GetName() const {
            return myname;
        }
        DoubleType GetCurrent() {
//          calcCurrent();
            return current;
        }
        DoubleType GetCharge() {
//          calcCharge();
            return charge;
        }
        const Contact &GetContact() const
        {
            return *mycontact;
        }

        const std::string &GetContactName() const;

        const std::string &GetRegionName() const;

        const std::string &GetDeviceName() const;

        ConstNodeList_t GetActiveNodes() const;

        //// This is how we will get our equation index and stuff for flux
        void SetCircuitNode(const std::string &);

        const std::string &GetCircuitNode() const;

        void UpdateContact();

        const Region &GetRegion() const
        {
            return *myregion;
        }

        void DevsimSerialize(std::ostream &) const;
        void GetCommandOptions(std::map<std::string, ObjectHolder> &) const;

    protected:

        virtual void Serialize(std::ostream &) const = 0;
        virtual void GetCommandOptions_Impl(std::map<std::string, ObjectHolder> &) const = 0;

        void SetCurrent(DoubleType x) {
            current = x;
        }
        void SetCharge(DoubleType x) {
            charge = x;
        }

        DoubleType integrateNodeModelOverNodes(const std::string &/*nmodel*/, const std::string &/*node_volume*/);
        DoubleType integrateEdgeModelOverNodes(const std::string &/*emodel*/, const std::string &/*edge_couple*/, const DoubleType /*n0_sign*/, const DoubleType /*n1_sign*/);
        DoubleType integrateElementEdgeModelOverNodes(const std::string &/*temodel*/, const std::string &/*edge_couple*/, const DoubleType /*n0_sign*/, const DoubleType /*n1_sign*/);
        DoubleType integrateTriangleEdgeModelOverNodes(const std::string &/*temodel*/, const std::string &/*edge_couple*/, const DoubleType /*n0_sign*/, const DoubleType /*n1_sign*/);
        DoubleType integrateTetrahedronEdgeModelOverNodes(const std::string &/*temodel*/, const std::string &/*edge_couple*/, const DoubleType /*n0_sign*/, const DoubleType /*n1_sign*/);

        void AssembleNodeEquation(const std::string &, dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, PermutationMap &, dsMathEnum::WhatToLoad, const std::string &);
        void AssembleEdgeEquation(const std::string &, dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, dsMathEnum::WhatToLoad, const std::string &, const DoubleType /*n0_sign*/, const DoubleType /*n1_sign*/);
        void AssembleElementEdgeEquation(const std::string &, dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, dsMathEnum::WhatToLoad, const std::string &, const DoubleType /*n0_sign*/, const DoubleType /*n1_sign*/);
        void AssembleTriangleEdgeEquation(const std::string &, dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, dsMathEnum::WhatToLoad, const std::string &, const DoubleType /*n0_sign*/, const DoubleType /*n1_sign*/);
        void AssembleTetrahedronEdgeEquation(const std::string &, dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, dsMathEnum::WhatToLoad, const std::string &, const DoubleType /*n0_sign*/, const DoubleType /*n1_sign*/);

        void AssembleNodeEquationOnCircuit(const std::string &, dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, dsMathEnum::WhatToLoad, const std::string &);
        void AssembleEdgeEquationOnCircuit(const std::string &, dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, dsMathEnum::WhatToLoad, const std::string &, const DoubleType /*n0_sign*/, const DoubleType /*n1_sign*/);
        void AssembleElementEdgeEquationOnCircuit(const std::string &, dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, dsMathEnum::WhatToLoad, const std::string &, const DoubleType /*n0_sign*/, const DoubleType /*n1_sign*/);
        void AssembleTriangleEdgeEquationOnCircuit(const std::string &, dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, dsMathEnum::WhatToLoad, const std::string &, const DoubleType /*n0_sign*/, const DoubleType /*n1_sign*/);
        void AssembleTetrahedronEdgeEquationOnCircuit(const std::string &, dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, dsMathEnum::WhatToLoad, const std::string &, const DoubleType /*n0_sign*/, const DoubleType /*n1_sign*/);


    private:
        virtual void DerivedAssemble(dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, PermutationMap &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode) = 0;

        virtual void calcCurrent() = 0;
        virtual void calcCharge() = 0;

        ContactEquation();
        ContactEquation(const ContactEquation &);
        ContactEquation &operator=(const ContactEquation &);

        std::string myname;
        std::string circuitnode;
        ContactPtr mycontact;
        RegionPtr  myregion;
        DoubleType charge;
        DoubleType current;
};
#endif

