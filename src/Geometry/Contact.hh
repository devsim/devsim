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

#ifndef CONTACT_HH
#define CONTACT_HH
#include "MathEnum.hh"
#include <utility>
#include <string>
#include <vector>
#include <map>
class Node;
typedef Node *NodePtr;
typedef std::vector<const Node *> ConstNodeList_t;
class Edge;
typedef Edge *EdgePtr;
typedef std::vector<const Edge *> ConstEdgeList_t;
class Triangle;
typedef Triangle *TrianglePtr;
typedef std::vector<const Triangle *> ConstTriangleList_t;


class Region;
typedef Region *RegionPtr;
typedef const Region *ConstRegionPtr;

class ContactEquation;
typedef ContactEquation *ContactEquationPtr;
typedef const ContactEquation *ConstContactEquationPtr;
typedef std::map<std::string, ContactEquationPtr> ContactEquationPtrMap_t;

typedef std::map<std::string, std::string> VariableEqnMap_t;


class PermutationEntry;
typedef std::map<size_t, PermutationEntry> PermutationMap;
namespace dsMath {
template <typename T> class RowColVal;
typedef std::vector<RowColVal<double> > RealRowColValueVec;
typedef std::pair<int, double> RHSEntry;
typedef std::vector<RHSEntry> RHSEntryVec;
}

// Create const list
class Contact {
    public:
      typedef std::map<std::string, ContactEquationPtr> ContactEquationPtrMap_t;

      ~Contact();
      Contact(const std::string &, RegionPtr, const ConstNodeList_t &/*n*/, const std::string &/*material*/);

      ConstRegionPtr     GetRegion() const;
      const std::string &GetName() const;
      const std::string &GetMaterialName() const;
      void  SetMaterial(const std::string &/*material_name*/);
      const ConstNodeList_t     &GetNodes() const;
      const ConstEdgeList_t     &GetEdges() const;
      const ConstTriangleList_t &GetTriangles() const;

      const std::string &GetDeviceName() const;

      void AddEquation(ContactEquationPtr);
      void DeleteEquation(ContactEquationPtr);
      ContactEquationPtrMap_t &GetEquationPtrList();
      const ContactEquationPtrMap_t &GetEquationPtrList() const;

      void Assemble(dsMath::RealRowColValueVec &, dsMath::RHSEntryVec &, PermutationMap &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);

    private:
        void FindEdges() const;
        void FindTriangles() const;

        Contact();
        Contact(const Contact &);
        Contact &operator=(const Contact &);
        std::string             name;
        std::string             materialName;
        RegionPtr               region;
        ConstNodeList_t         contactnodes;
        mutable ConstEdgeList_t     contactedges;
        mutable ConstTriangleList_t contacttriangles;
        ContactEquationPtrMap_t contactEquationPtrMap;
        VariableEqnMap_t        variableEquationMap;
};
#endif
