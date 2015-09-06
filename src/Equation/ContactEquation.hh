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
typedef RowColVal<double> RealRowColVal;
typedef std::vector<RowColVal<double> > RealRowColValueVec;
}

typedef std::pair<int, double> RHSEntry;
typedef std::vector<RHSEntry> RHSEntryVec;
// Very similar to a regular equation, but with additional properties
class ContactEquation {
    public:
        ContactEquation(const std::string &/*eqn*/, const std::string &/*var*/, ContactPtr, RegionPtr);
        virtual ~ContactEquation() = 0;
        void Assemble(dsMath::RealRowColValueVec &, RHSEntryVec &, PermutationMap &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);
        const std::string &GetName() const {
            return myname;
        }
        double GetCurrent() {
//          calcCurrent();
            return current;
        }
        double GetCharge() {
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

        const std::string &GetVariable() const
        {
          return variable;
        }

        void DevsimSerialize(std::ostream &) const;
        void GetCommandOptions(std::map<std::string, ObjectHolder> &) const;

    protected:

        virtual void Serialize(std::ostream &) const = 0;
        virtual void GetCommandOptions_Impl(std::map<std::string, ObjectHolder> &) const = 0;

        void SetCurrent(double x) {
            current = x;
        }
        void SetCharge(double x) {
            charge = x;
        }

        double integrateNodeModelOverNodes(const std::string &/*nmodel*/, const std::string &/*node_volume*/);
        double integrateEdgeModelOverNodes(const std::string &/*emodel*/, const std::string &/*edge_couple*/);
        double integrateElementEdgeModelOverNodes(const std::string &/*temodel*/, const std::string &/*edge_couple*/, const double /*n0_sign*/, const double /*n1_sign*/);
        double integrateTriangleEdgeModelOverNodes(const std::string &/*temodel*/, const std::string &/*edge_couple*/, const double /*n0_sign*/, const double /*n1_sign*/);
        double integrateTetrahedronEdgeModelOverNodes(const std::string &/*temodel*/, const std::string &/*edge_couple*/, const double /*n0_sign*/, const double /*n1_sign*/);

        void AssembleNodeEquation(const std::string &, dsMath::RealRowColValueVec &, RHSEntryVec &, PermutationMap &, dsMathEnum::WhatToLoad, const std::string &);
        void AssembleEdgeEquation(const std::string &, dsMath::RealRowColValueVec &, RHSEntryVec &, dsMathEnum::WhatToLoad, const std::string &);
        void AssembleElementEdgeEquation(const std::string &, dsMath::RealRowColValueVec &, RHSEntryVec &, dsMathEnum::WhatToLoad, const std::string &, const double /*n0_sign*/, const double /*n1_sign*/);
        void AssembleTriangleEdgeEquation(const std::string &, dsMath::RealRowColValueVec &, RHSEntryVec &, dsMathEnum::WhatToLoad, const std::string &, const double /*n0_sign*/, const double /*n1_sign*/);
        void AssembleTetrahedronEdgeEquation(const std::string &, dsMath::RealRowColValueVec &, RHSEntryVec &, dsMathEnum::WhatToLoad, const std::string &, const double /*n0_sign*/, const double /*n1_sign*/);

        void AssembleNodeEquationOnCircuit(const std::string &, dsMath::RealRowColValueVec &, RHSEntryVec &, dsMathEnum::WhatToLoad, const std::string &);
        void AssembleEdgeEquationOnCircuit(const std::string &, dsMath::RealRowColValueVec &, RHSEntryVec &, dsMathEnum::WhatToLoad, const std::string &);
        void AssembleElementEdgeEquationOnCircuit(const std::string &, dsMath::RealRowColValueVec &, RHSEntryVec &, dsMathEnum::WhatToLoad, const std::string &, const double /*n0_sign*/, const double /*n1_sign*/);
        void AssembleTriangleEdgeEquationOnCircuit(const std::string &, dsMath::RealRowColValueVec &, RHSEntryVec &, dsMathEnum::WhatToLoad, const std::string &, const double /*n0_sign*/, const double /*n1_sign*/);
        void AssembleTetrahedronEdgeEquationOnCircuit(const std::string &, dsMath::RealRowColValueVec &, RHSEntryVec &, dsMathEnum::WhatToLoad, const std::string &, const double /*n0_sign*/, const double /*n1_sign*/);


    private:
        virtual void DerivedAssemble(dsMath::RealRowColValueVec &, RHSEntryVec &, PermutationMap &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode) = 0;

        virtual void calcCurrent() = 0;
        virtual void calcCharge() = 0;

        ContactEquation();
        ContactEquation(const ContactEquation &);
        ContactEquation &operator=(const ContactEquation &);

        std::string myname;
        std::string variable;
        std::string circuitnode;
        ContactPtr mycontact;
        RegionPtr  myregion;
        double charge;
        double current;
};
#endif

