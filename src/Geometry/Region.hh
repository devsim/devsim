/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef REGION_HH
#define REGION_HH
#include "MathEnum.hh"

#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
typedef float128 extended_type;
#else
typedef double extended_type;
#endif
#include "dsMathTypes.hh"

#include <memory>

#include <cstddef>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <complex>

class PermutationEntry;

template <typename T>
class Vector;

template <typename T>
class GradientField;
template <typename T>
class TriangleElementField;
template <typename T>
class TetrahedronElementField;

template <typename T> class ObjectCache;
namespace MEE {
template <typename DoubleType>
class ModelExprData;
}

template <typename DoubleType>
using ModelExprDataCache = ObjectCache<MEE::ModelExprData<DoubleType> >;

template <typename DoubleType>
using WeakModelExprDataCachePtr = std::weak_ptr<ModelExprDataCache<DoubleType> >;

template <typename DoubleType>
using ModelExprDataCachePtr = std::shared_ptr<ModelExprDataCache<DoubleType> >;

class Device;
typedef Device *DevicePtr;
typedef const Device *ConstDevicePtr;

class NodeModel;
using WeakNodeModelPtr = std::weak_ptr<NodeModel>;
using NodeModelPtr = std::shared_ptr<NodeModel>;
using ConstNodeModelPtr = std::shared_ptr<const NodeModel>;

class EdgeModel;
using WeakEdgeModelPtr = std::weak_ptr<EdgeModel>;
using EdgeModelPtr = std::shared_ptr<EdgeModel>;
using ConstEdgeModelPtr = std::shared_ptr<const EdgeModel>;

class TriangleEdgeModel;
using WeakTriangleEdgeModelPtr = std::weak_ptr<TriangleEdgeModel>;
using TriangleEdgeModelPtr = std::shared_ptr<TriangleEdgeModel>;
using ConstTriangleEdgeModelPtr = std::shared_ptr<const TriangleEdgeModel>;

class TetrahedronEdgeModel;
using WeakTetrahedronEdgeModelPtr = std::weak_ptr<TetrahedronEdgeModel>;
using TetrahedronEdgeModelPtr = std::shared_ptr<TetrahedronEdgeModel>;
using ConstTetrahedronEdgeModelPtr = std::shared_ptr<const TetrahedronEdgeModel>;


class Edge;
typedef Edge *EdgePtr;
typedef const Edge *ConstEdgePtr;
typedef std::vector<ConstEdgePtr> ConstEdgeList;


class Node;
typedef Node *NodePtr;
typedef const Node *ConstNodePtr;
typedef std::vector<NodePtr> NodeList;
typedef std::vector<ConstNodePtr> ConstNodeList;

class Triangle;
typedef Triangle *TrianglePtr;
typedef const Triangle *ConstTrianglePtr;
typedef std::vector<ConstTrianglePtr> ConstTriangleList;

//// Used for knowing the two Triangles on each Tetrahedron edge
class EdgeData;
typedef const EdgeData *ConstEdgeDataPtr;
typedef std::vector<ConstEdgeDataPtr> ConstEdgeDataList;


class Tetrahedron;
typedef Tetrahedron *TetrahedronPtr;
typedef const Tetrahedron *ConstTetrahedronPtr;
typedef std::vector<ConstTetrahedronPtr> ConstTetrahedronList;

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

typedef std::map<std::string, size_t> EquationIndMap_t;

class EquationHolder;

typedef std::map<std::string, EquationHolder> EquationPtrMap_t;
// Variable to equation
typedef std::map<std::string, std::string> VariableEqnMap_t;

typedef std::vector<std::string> VariableList_t;


/**
   A Region knows which mesh it belongs to.  A mesh may contain a shared_ptr to
   it.  A node may contain a weak_ptr pointing back to the region which
   contains it.

   if the numberNodes_ < maxNodeIndex_, then we will recycle free node indexes
   for reuse
*/
class Region
{
      enum {DEFAULT_NUMBER_NODES=1024};
   public:
      typedef std::vector<ConstEdgeList> NodeToConstEdgeList_t;

      typedef std::vector<ConstTriangleList> NodeToConstTriangleList_t;
      typedef std::vector<ConstTriangleList> EdgeToConstTriangleList_t;
      typedef std::vector<ConstEdgeList>     TriangleToConstEdgeList_t;

      typedef std::vector<ConstTetrahedronList> NodeToConstTetrahedronList_t;
      typedef std::vector<ConstTetrahedronList> EdgeToConstTetrahedronList_t;
      typedef std::vector<ConstEdgeDataList>    TetrahedronToConstEdgeDataList_t;
      typedef std::vector<ConstTriangleList>    TetrahedronToConstTriangleList_t;
      typedef std::vector<ConstTetrahedronList> TriangleToConstTetrahedronList_t;

      typedef std::map<std::string, TriangleEdgeModelPtr> TriangleEdgeModelList_t;
      typedef std::map<std::string, TetrahedronEdgeModelPtr> TetrahedronEdgeModelList_t;
      typedef std::map<std::string, EdgeModelPtr> EdgeModelList_t;
      typedef std::map<std::string, NodeModelPtr> NodeModelList_t;
      typedef std::map<std::string, std::set<std::string> > DependencyMap_t;

      Region(std::string, std::string, size_t, ConstDevicePtr);
      ~Region();
      void AddNode(const NodePtr &);
      void AddNodeList(ConstNodeList &);

      void AddEdge(const EdgePtr &);
      void AddEdgeList(ConstEdgeList &);

      void AddTriangle(const TrianglePtr &);
      void AddTriangleList(ConstTriangleList &);

      void AddTetrahedron(const TetrahedronPtr &);
      void AddTetrahedronList(ConstTetrahedronList &);


      void FinalizeMesh();

      size_t GetNumberNodes() const {
         return nodeList.size();
      }

      size_t GetNumberEdges() const {
         return edgeList.size();
      }

      size_t GetNumberTriangles() const {
         return triangleList.size();
      }

      size_t GetNumberTetrahedrons() const {
         return tetrahedronList.size();
      }

      size_t GetDimension() const {
          return dimension;
      }
      const std::string &GetName() const {
          return regionName;
      }

      const std::string &GetDeviceName() const;

      const std::string &GetMaterialName() const {
          return materialName;
      }

      void SetMaterial(const std::string &/*material_name*/);

      const ConstEdgeList &GetEdgeList() const {
        return edgeList;
      }

      const ConstNodeList &GetNodeList() const {
        return nodeList;
      }

      const ConstTriangleList &GetTriangleList() const {
        return triangleList;
      }

      const ConstTetrahedronList &GetTetrahedronList() const {
        return tetrahedronList;
      }

      const NodeToConstEdgeList_t  &GetNodeToEdgeList() const {
        return nodeToEdgeList;
      }

      const NodeToConstTriangleList_t &GetNodeToTriangleList() const {
        return nodeToTriangleList;
      }

      const EdgeToConstTriangleList_t &GetEdgeToTriangleList() const {
        return edgeToTriangleList;
      }

      const TriangleToConstEdgeList_t &GetTriangleToEdgeList() const {
        return triangleToEdgeList;
      }

      const NodeToConstTetrahedronList_t &GetNodeToTetrahedronList() const {
        return nodeToTetrahedronList;
      }

      const EdgeToConstTetrahedronList_t &GetEdgeToTetrahedronList() const {
        return edgeToTetrahedronList;
      }

      const TetrahedronToConstEdgeDataList_t &GetTetrahedronToEdgeDataList() const {
        return tetrahedronToEdgeDataList;
      }

      const TriangleToConstTetrahedronList_t &GetTriangleToTetrahedronList() const {
        return triangleToTetrahedronList;
      }

      const TetrahedronToConstTriangleList_t &GetTetrahedronToTriangleList() const {
        return tetrahedronToTriangleList;
      }

      // Methods to search by node ptr
      ConstEdgePtr FindEdge(ConstNodePtr, ConstNodePtr) const;
      ConstTrianglePtr FindTriangle(ConstNodePtr, ConstNodePtr, ConstNodePtr) const;
      ConstTetrahedronPtr FindTetrahedron(ConstNodePtr, ConstNodePtr, ConstNodePtr, ConstNodePtr) const;


      // try using a multimap first
      void RegisterCallback(const std::string &, const std::string &);
      // model can signal dependencies, passing this
      // Note that a NodeModelPtr can signal both EdgeModels and NodeModels
      void SignalCallbacks(const std::string &);

      // unregister a model when it is destructed
      void UnregisterCallback(const std::string &);

      // note that these can be used to alias the same model with multiple names
      NodeModelPtr AddNodeModel(NodeModel *);
      EdgeModelPtr AddEdgeModel(EdgeModel *);
      TriangleEdgeModelPtr AddTriangleEdgeModel(TriangleEdgeModel *);
      TetrahedronEdgeModelPtr  AddTetrahedronEdgeModel(TetrahedronEdgeModel *);
      // given a models name, spit out a pointer to it

      ConstNodeModelPtr    GetNodeModel(const std::string &) const;
      ConstEdgeModelPtr            GetEdgeModel(const std::string &) const;
      ConstTriangleEdgeModelPtr    GetTriangleEdgeModel(const std::string &) const;
      ConstTetrahedronEdgeModelPtr GetTetrahedronEdgeModel(const std::string &) const;

      void DeleteNodeModel(const std::string &);

      void DeleteEdgeModel(const std::string &);

      void DeleteTriangleEdgeModel(const std::string &);

      void DeleteTetrahedronEdgeModel(const std::string &);

      const EdgeModelList_t &GetEdgeModelList() const {
        return edgeModels;
      }

      const NodeModelList_t &GetNodeModelList() const {
        return nodeModels;
      }

      const TriangleEdgeModelList_t &GetTriangleEdgeModelList() const {
        return triangleEdgeModels;
      }

      const TetrahedronEdgeModelList_t &GetTetrahedronEdgeModelList() const {
        return tetrahedronEdgeModels;
      }

      void AddEquation(EquationHolder &);
      void DeleteEquation(EquationHolder &);
      size_t GetEquationIndex(const std::string &) const;
      // Is access control worth it?
      // Fix this
      EquationPtrMap_t &GetEquationPtrList();
      const EquationPtrMap_t &GetEquationPtrList() const;
      // also create Get equation names
      std::string GetEquationNameFromVariable(const std::string &) const;

      size_t GetEquationNumber(size_t /*equation index*/, ConstNodePtr) const;
      void SetBaseEquationNumber(size_t);
      size_t GetBaseEquationNumber() const;
      size_t GetNumberEquations() const;
      size_t GetMaxEquationNumber() const;

      bool operator==(const Region &) const;

      ConstDevicePtr GetDevice() const;

      VariableList_t GetVariableList() const;

    template <typename DoubleType>
    DoubleType GetAbsError() const
    {
        return static_cast<DoubleType>(absError);
    }

    template <typename DoubleType>
    DoubleType GetRelError() const
    {
        return static_cast<DoubleType>(relError);
    }

    template <typename DoubleType>
    void Update(const dsMath::DoubleVec_t<DoubleType> &/*result*/);
    template <typename DoubleType>
    void ACUpdate(const dsMath::ComplexDoubleVec_t<DoubleType> &/*result*/);
    template <typename DoubleType>
    void NoiseUpdate(const std::string &/*output*/, const std::vector<PermutationEntry> &/*permvec*/, const dsMath::ComplexDoubleVec_t<DoubleType> &/*result*/);

    template <typename DoubleType>
    void Assemble(dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);

    void BackupSolutions(const std::string &);
    void RestoreSolutions(const std::string &);

    template <typename DoubleType>
    const GradientField<DoubleType> &GetGradientField() const;

    template <typename DoubleType>
    const TriangleElementField<DoubleType> &GetTriangleElementField() const;

    template <typename DoubleType>
    const TetrahedronElementField<DoubleType> &GetTetrahedronElementField() const;

    template <typename DoubleType>
    const std::vector<Vector<DoubleType> > &GetTriangleCenters() const;

    template <typename DoubleType>
    const std::vector<Vector<DoubleType> > &GetTetrahedronCenters() const;

    size_t GetEdgeIndexOnTriangle(const Triangle &, ConstEdgePtr) const;

    size_t GetEdgeIndexOnTetrahedron(const Tetrahedron &, ConstEdgePtr) const;

    std::string GetNodeVolumeModel() const;
    std::string GetEdgeCoupleModel() const;
    std::string GetElementEdgeCoupleModel() const;
    std::string GetEdgeNode0VolumeModel() const;
    std::string GetEdgeNode1VolumeModel() const;
    std::string GetElementNode0VolumeModel() const;
    std::string GetElementNode1VolumeModel() const;


    //// const in the sense we are not changing the region
    //// however the cache is changeable
    template <typename DoubleType>
    ModelExprDataCachePtr<DoubleType> GetModelExprDataCache();

    template <typename DoubleType>
    void SetModelExprDataCache(ModelExprDataCachePtr<DoubleType>);

    bool UseExtendedPrecisionModels() const;
    bool UseExtendedPrecisionEquations() const;
   private:
      Region();
      Region (const Region &);
      Region &operator= (const Region &);

      void SetNodeIndexes();
      void SetEdgeIndexes();
      void SetTriangleIndexes();
      void SetTetrahedronIndexes();

      void CreateNodeToEdgeList();

      void CreateNodeToTriangleList();
      void CreateEdgeToTriangleList();
      void CreateTriangleToEdgeList();

      void CreateNodeToTetrahedronList();
      void CreateEdgeToTetrahedronList();
      void CreateTetrahedronToEdgeDataList();
      void CreateTetrahedronToTriangleList();
      void CreateTriangleToTetrahedronList();
      void SetTriangleCenters();
      void SetTetrahedronCenters();

      bool UseExtendedPrecisionType(const std::string &t) const;

      template <typename DoubleType>
      struct GeometryField
      {
        GeometryField() : gradientField(nullptr), triangleElementField(nullptr), tetrahedronElementField(nullptr)
        {
        }

        //  put implementation in the c
        ~GeometryField();

        mutable GradientField<DoubleType>           *gradientField;
        mutable TriangleElementField<DoubleType>    *triangleElementField;
        mutable TetrahedronElementField<DoubleType> *tetrahedronElementField;
        std::vector<Vector<DoubleType> >             triangleCenters;
        std::vector<Vector<DoubleType> >             tetrahedronCenters;
      };

      template <typename DoubleType>
      GeometryField<DoubleType> &GetGeometryField() const;

      std::string materialName;
      std::string regionName;
      size_t dimension; //1D, 2D, or 3D mesh

      EquationIndMap_t equationIndexMap;
      EquationPtrMap_t equationPointerMap;
      VariableEqnMap_t variableEquationMap;

      ConstNodeList        nodeList;
      ConstEdgeList        edgeList;
      ConstTriangleList    triangleList;
      ConstTetrahedronList tetrahedronList;

      // one for one correspondence with nodeList
      NodeToConstEdgeList_t     nodeToEdgeList;

      NodeToConstTriangleList_t nodeToTriangleList;
      EdgeToConstTriangleList_t edgeToTriangleList;
      TriangleToConstEdgeList_t triangleToEdgeList;

      NodeToConstTetrahedronList_t     nodeToTetrahedronList;
      EdgeToConstTetrahedronList_t     edgeToTetrahedronList;
      TetrahedronToConstEdgeDataList_t tetrahedronToEdgeDataList;
      TetrahedronToConstTriangleList_t tetrahedronToTriangleList;
      TriangleToConstTetrahedronList_t triangleToTetrahedronList;

      NodeModelList_t            nodeModels;
      EdgeModelList_t            edgeModels;
      TriangleEdgeModelList_t    triangleEdgeModels;
      TetrahedronEdgeModelList_t tetrahedronEdgeModels;

      DependencyMap_t DependencyMap;

      size_t baseeqnnum; // base equation number for this region
      size_t numequations;
      bool   finalized;
      ConstDevicePtr device;
      const   std::string deviceName;
#ifdef DEVSIM_EXTENDED_PRECISION
      float128  relError;
      float128  absError;
#else
      double  relError;
      double  absError;
#endif

      mutable GeometryField<double> GeometryField_double;
#ifdef DEVSIM_EXTENDED_PRECISION
      mutable GeometryField<float128> GeometryField_float128;
#endif

      WeakModelExprDataCachePtr<double> modelExprDataCache_double;
#ifdef DEVSIM_EXTENDED_PRECISION
      WeakModelExprDataCachePtr<float128> modelExprDataCache_float128;
#endif
};

#endif

