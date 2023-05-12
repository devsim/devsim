/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef TETRAHEDRON_EDGE_MODEL_HH
#define TETRAHEDRON_EDGE_MODEL_HH

#include "ModelDataHolder.hh"

#include <memory>

#include <string>
#include <vector>
#include <iosfwd>

template<typename T>
using TetrahedronEdgeScalarList = std::vector<T>;

template<typename T>
using NodeScalarList = std::vector<T>;

template<typename T>
using EdgeScalarList = std::vector<T>;


class Region;
typedef Region *RegionPtr;

class Edge;
typedef Edge *EdgePtr;
typedef const Edge *ConstEdgePtr;

class TetrahedronEdgeModel;
using WeakTetrahedronEdgeModelPtr = std::weak_ptr<TetrahedronEdgeModel>;
using WeakConstTetrahedronEdgeModelPtr = std::weak_ptr<const TetrahedronEdgeModel>;
using TetrahedronEdgeModelPtr = std::shared_ptr<TetrahedronEdgeModel>;
using ConstTetrahedronEdgeModelPtr = std::shared_ptr<const TetrahedronEdgeModel>;

class Tetrahedron;
typedef Tetrahedron *TetrahedronPtr;
typedef const Tetrahedron *ConstTetrahedronPtr;

class TetrahedronEdgeModel {
    public:
        enum class DisplayType {NODISPLAY, SCALAR, UNKNOWN};

        TetrahedronEdgeModel(const std::string &, const RegionPtr, TetrahedronEdgeModel::DisplayType);
        virtual ~TetrahedronEdgeModel();

        const std::string &GetName() const {
            return name;
        }

        template <typename DoubleType>
        const TetrahedronEdgeScalarList<DoubleType> &GetScalarValues() const;

        enum class InterpolationType {AVERAGE, COUPLE, SUM};

        template <typename DoubleType>
        void GetScalarValuesOnNodes(TetrahedronEdgeModel::InterpolationType, std::vector<DoubleType> &) const;

        template <typename DoubleType>
        void GetScalarValuesOnElements(std::vector<DoubleType> &) const;

        template <typename DoubleType>
        EdgeScalarList<DoubleType> GetValuesOnEdges() const;

        void MarkOld();

        bool IsUpToDate() const
        {
            return uptodate;
        }

        /// Use this to break cycles
        /// Only really valid in context of ExprModels.
        bool IsInProcess() const
        {
            return inprocess;
        }

        template <typename DoubleType>
        void SetValues(const TetrahedronEdgeScalarList<DoubleType> &);

        template <typename DoubleType>
        void SetValues(const DoubleType &);

        const Region &GetRegion() const
        {
            return *myregion;
        }

        TetrahedronEdgeModel::DisplayType GetDisplayType() const
        {
          return displayType;
        }

        const char * GetDisplayTypeString() const
        {
          return DisplayTypeString[static_cast<size_t>(displayType)];
        }

        void SetDisplayType(TetrahedronEdgeModel::DisplayType dt)
        {
          displayType = dt;
        }

        ConstTetrahedronEdgeModelPtr GetConstSelfPtr() const
        {
          return myself.lock();
        }

        TetrahedronEdgeModelPtr GetSelfPtr()
        {
          return myself.lock();
        }

        bool IsUniform() const;

        template <typename DoubleType>
        const DoubleType &GetUniformValue() const;

        size_t GetLength() const
        {
          return model_data.GetLength();
        }

        bool IsZero() const;

        bool IsOne() const;

        void DevsimSerialize(std::ostream &) const;

        const std::string &GetRegionName() const;

        const std::string &GetDeviceName() const;

    protected:
        virtual void Serialize(std::ostream &) const = 0;
        void SerializeBuiltIn(std::ostream &) const;

        void RegisterCallback(const std::string &);

        template <typename DoubleType>
        void SetValues(const TetrahedronEdgeScalarList<DoubleType> &) const;
        template <typename DoubleType>
        void SetValues(const DoubleType &) const;

        void MarkOld() const;

    private:
        void CalculateValues() const;

        // Actually performs the computation
        // The nonvirtual method does any required setup.
        //virtual DoubleType calcEdgeScalarValue(EdgePtr) const = 0;
        virtual void calcTetrahedronEdgeScalarValues() const = 0;


        TetrahedronEdgeModel();
        TetrahedronEdgeModel(const TetrahedronEdgeModel &);
        TetrahedronEdgeModel &operator=(const TetrahedronEdgeModel &);

        // required for models that store their data
        // (some models may be created on the fly)
//      EdgeList data;
        std::string name;
        WeakTetrahedronEdgeModelPtr myself;
        // need to know my region to get database data and appropriate node and edge lists
        RegionPtr   myregion;
        mutable bool uptodate;
        mutable bool inprocess;
        DisplayType displayType;
        mutable ModelDataHolder model_data;
        static const char *DisplayTypeString[];
};

template <typename T1, typename T2, typename ... Args>
TetrahedronEdgeModelPtr create_tetrahedron_edge_model(bool use_extended, Args &&...args)
{
  TetrahedronEdgeModel *ret;
  if (use_extended)
  {
    ret = new T2(std::forward<Args>(args)...);
  }
  else
  {
    ret = new T1(std::forward<Args>(args)...);
  }
  return ret->GetSelfPtr();
}
#endif

