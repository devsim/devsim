/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef INTERFACE_HH
#define INTERFACE_HH
#include "MathEnum.hh"
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
#endif
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>

template <typename T> class ObjectCache;
namespace IMEE {
template <typename T>
class InterfaceModelExprData;
}

template <typename DoubleType>
using InterfaceModelExprDataCache = ObjectCache<IMEE::InterfaceModelExprData<DoubleType> >;

template <typename DoubleType>
using WeakInterfaceModelExprDataCachePtr =  std::weak_ptr<InterfaceModelExprDataCache<DoubleType> >;

template <typename DoubleType>
using InterfaceModelExprDataCachePtr = std::shared_ptr<InterfaceModelExprDataCache<DoubleType> >;

class Node;
class Edge;
class Triangle;

class Region;
typedef Region *RegionPtr;
typedef const Region *ConstRegionPtr;

class InterfaceNodeModel;
using InterfaceNodeModelPtr = std::shared_ptr<InterfaceNodeModel>;
using ConstInterfaceNodeModelPtr = std::shared_ptr<const InterfaceNodeModel>;

class InterfaceEquationHolder;
typedef std::map<std::string, InterfaceEquationHolder> InterfaceEquationPtrMap_t;

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



/// All nodes on an interface share the same nodes
/// todo: somehow regions must signal when they are deleted so the Interface knows it is invalid (perhaps using callback)
class Interface {
    public:
        typedef std::vector<const Node *>     ConstNodeList_t;
        typedef std::vector<const Edge *>     ConstEdgeList_t;
        typedef std::vector<const Triangle *> ConstTriangleList_t;
        typedef std::map<std::string, InterfaceNodeModelPtr> NameToInterfaceNodeModelMap_t;
        typedef std::map<std::string, std::set<std::string> > DependencyMap_t;

        ~Interface();

        Interface(const std::string &, Region *, Region *, const ConstNodeList_t &, const ConstNodeList_t &);

        ConstRegionPtr GetRegion0() const {
            return rp0;
        }
        ConstRegionPtr GetRegion1() const {
            return rp1;
        }

        const std::string &GetDeviceName() const;

        const ConstNodeList_t &GetNodes0() const {
          return nodes0;
        }

        const ConstNodeList_t &GetNodes1() const {
          return nodes1;
        }

        const ConstEdgeList_t &GetEdges0() const;
        const ConstEdgeList_t &GetEdges1() const;

        const ConstTriangleList_t &GetTriangles0() const;
        const ConstTriangleList_t &GetTriangles1() const;

        const std::string &GetName() const {
            return name;
        }

        void AddInterfaceEquation(InterfaceEquationHolder &);
        void DeleteInterfaceEquation(InterfaceEquationHolder &);
        InterfaceEquationPtrMap_t &GetInterfaceEquationList();
        const InterfaceEquationPtrMap_t &GetInterfaceEquationList() const;

        // try using a multimap first
        void RegisterCallback(const std::string &, const std::string &);
        // model can signal dependencies, passing this
        // Note that a NodeModelPtr can signal both EdgeModels and NodeModels
        void SignalCallbacks(const std::string &);

        void SignalCallbacks(const std::string &, ConstRegionPtr /*rp*/);

        // unregister a model when it is destructed
        void UnregisterCallback(const std::string &);


        /// passes ownership
        void AddInterfaceNodeModel(InterfaceNodeModel *);
        ConstInterfaceNodeModelPtr GetInterfaceNodeModel(const std::string &) const;

        void DeleteInterfaceNodeModel(const std::string &);

        const NameToInterfaceNodeModelMap_t &GetInterfaceNodeModelList() const;

        template <typename DoubleType>
        void Assemble(dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, PermutationMap &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);

        void AddEdges(const ConstEdgeList_t &, const ConstEdgeList_t &);
        void AddTriangles(const ConstTriangleList_t &, const ConstTriangleList_t &);

        bool operator==(const Interface &oi) const
        {
          return (&oi) == this;
        }

        std::string GetSurfaceAreaModel() const;

        template <typename DoubleType>
        InterfaceModelExprDataCachePtr<DoubleType> GetInterfaceModelExprDataCache();

        template <typename DoubleType>
        void SetInterfaceModelExprDataCache(InterfaceModelExprDataCachePtr<DoubleType>);

        bool UseExtendedPrecisionModels() const;
        bool UseExtendedPrecisionEquations() const;

        bool ElementsProvided() const
        {
          return elements_provided_;
        }

    private:
        void FindEdges() const;
        void FindTriangles() const;
        bool UseExtendedPrecisionType(const std::string &) const;

        Interface();
        Interface(const Interface &);
        Interface & operator=(const Interface &);

        std::string name;
        RegionPtr rp0;
        RegionPtr rp1;
        ConstNodeList_t     nodes0;  // Pointers to nodes in region 0
        ConstNodeList_t     nodes1;  // Pointers to nodes in region 1
        //// Note the edges and triangles don't necessarily have the same ordering
        //// on both sides of the interface
        mutable ConstEdgeList_t     edges0;  // Pointers to edges in region 0
        mutable ConstEdgeList_t     edges1;  // Pointers to edges in region 1
        mutable ConstTriangleList_t triangles0;  // Pointers to triangles in region 0
        mutable ConstTriangleList_t triangles1;  // Pointers to triangles in region 1
        InterfaceEquationPtrMap_t interfaceEquationList;
        NameToInterfaceNodeModelMap_t interfaceNodeModels;

        DependencyMap_t DependencyMap;

        WeakInterfaceModelExprDataCachePtr<double>   interfaceModelExprDataCache_double;
#ifdef DEVSIM_EXTENDED_PRECISION
        WeakInterfaceModelExprDataCachePtr<float128> interfaceModelExprDataCache_float128;
#endif
        // Are the mesh elemenents provided, or do we start with the nodes
        mutable bool elements_provided_;
};
#endif

