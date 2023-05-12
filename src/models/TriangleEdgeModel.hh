/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef TRIANGLE_EDGE_MODEL_HH
#define TRIANGLE_EDGE_MODEL_HH

#include "ModelDataHolder.hh"

#include <memory>
#include <string>
#include <vector>
#include <iosfwd>

template<typename T>
using TriangleEdgeScalarList = std::vector<T>;

template<typename T>
using NodeScalarList = std::vector<T>;

template<typename T>
using EdgeScalarList = std::vector<T>;

class Region;
typedef Region *RegionPtr;

class Edge;
typedef Edge *EdgePtr;
typedef const Edge *ConstEdgePtr;

class TriangleEdgeModel;
using WeakTriangleEdgeModelPtr = std::weak_ptr<TriangleEdgeModel>;
using WeakConstTriangleEdgeModelPtr = std::weak_ptr<const TriangleEdgeModel>;
using TriangleEdgeModelPtr = std::shared_ptr<TriangleEdgeModel>;
using ConstTriangleEdgeModelPtr = std::shared_ptr<const TriangleEdgeModel>;

class Triangle;
typedef Triangle *TrianglePtr;
typedef const Triangle *ConstTrianglePtr;

class TriangleEdgeModel {
    public:
        enum class DisplayType {NODISPLAY, SCALAR, UNKNOWN};
        TriangleEdgeModel(const std::string &, const RegionPtr, TriangleEdgeModel::DisplayType);
        virtual ~TriangleEdgeModel();

        const std::string &GetName() const {
            return name;
        }

        template <typename DoubleType>
        const TriangleEdgeScalarList<DoubleType> &GetScalarValues() const;

        enum class InterpolationType {AVERAGE, COUPLE, SUM};

        template <typename DoubleType>
        void GetScalarValuesOnNodes(TriangleEdgeModel::InterpolationType, std::vector<DoubleType> &) const;

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
        void SetValues(const TriangleEdgeScalarList<DoubleType> &);

        template <typename DoubleType>
        void SetValues(const DoubleType &);

        const Region &GetRegion() const
        {
            return *myregion;
        }

        TriangleEdgeModel::DisplayType GetDisplayType() const
        {
          return displayType;
        }

        const char * GetDisplayTypeString() const
        {
          return DisplayTypeString[static_cast<size_t>(displayType)];
        }

        void SetDisplayType(TriangleEdgeModel::DisplayType dt)
        {
          displayType = dt;
        }

        ConstTriangleEdgeModelPtr GetConstSelfPtr() const
        {
          return myself.lock();
        }

        TriangleEdgeModelPtr GetSelfPtr()
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
        void SetValues(const TriangleEdgeScalarList<DoubleType> &) const;

        template <typename DoubleType>
        void SetValues(const DoubleType &) const;

        void MarkOld() const;

    private:
        void CalculateValues() const;

        // Actually performs the computation
        // The nonvirtual method does any required setup.
        //virtual DoubleType calcEdgeScalarValue(EdgePtr) const = 0;
        virtual void calcTriangleEdgeScalarValues() const = 0;


        TriangleEdgeModel();
        TriangleEdgeModel(const TriangleEdgeModel &);
        TriangleEdgeModel &operator=(const TriangleEdgeModel &);

        // required for models that store their data
        // (some models may be created on the fly)
//      EdgeList data;
        std::string name;
        WeakTriangleEdgeModelPtr myself;
        // need to know my region to get database data and appropriate node and edge lists
        RegionPtr   myregion;
        mutable bool uptodate;
        mutable bool inprocess;
        DisplayType displayType;
        mutable ModelDataHolder model_data;
        static const char *DisplayTypeString[];
};


template <typename T1, typename T2, typename ... Args>
TriangleEdgeModelPtr create_triangle_edge_model(bool use_extended, Args &&...args)
{
  TriangleEdgeModel *ret;
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

