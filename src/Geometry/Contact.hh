/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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

class ContactEquationHolder;
typedef std::map<std::string, ContactEquationHolder> ContactEquationPtrMap_t;

typedef std::map<std::string, std::string> VariableEqnMap_t;


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

// Create const list
class Contact {
    public:
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

      void AddEquation(ContactEquationHolder &);
      void DeleteEquation(ContactEquationHolder &);
      ContactEquationPtrMap_t &GetEquationPtrList();
      const ContactEquationPtrMap_t &GetEquationPtrList() const;

      template <typename DoubleType>
      void Assemble(dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, PermutationMap &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);

      void AddEdges(const ConstEdgeList_t &);
      void AddTriangles(const ConstTriangleList_t &);

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
