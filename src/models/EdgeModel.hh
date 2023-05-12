/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef EDGEMODEL_HH
#define EDGEMODEL_HH

#include "ModelDataHolder.hh"

#include "Vector.hh"

#include <memory>

#include <string>
#include <vector>
#include <iosfwd>

template<typename T>
using EdgeScalarList = std::vector<T>;

template<typename T>
using EdgeVectorList = std::vector<Vector<T> >;

template<typename T>
using NodeScalarList = std::vector<T>;

template<typename T>
using NodeVectorList = std::vector<Vector<T> >;

class EdgeModel;
using WeakEdgeModelPtr = std::weak_ptr<EdgeModel>;
using WeakConstEdgeModelPtr = std::weak_ptr<const EdgeModel>;
using EdgeModelPtr = std::shared_ptr<EdgeModel>;
using ConstEdgeModelPtr = std::shared_ptr<const EdgeModel>;

class Contact;
typedef Contact *ContactPtr;
typedef const Contact *ConstContactPtr;

class Region;
typedef Region *RegionPtr;

class Edge;
typedef Edge *EdgePtr;
typedef const Edge *ConstEdgePtr;

class EdgeModel {
    public:
        enum class DisplayType {NODISPLAY, SCALAR, VECTOR, UNKNOWN};
        EdgeModel(const std::string &, const RegionPtr, EdgeModel::DisplayType, const ContactPtr x = nullptr);
        virtual ~EdgeModel();

        const std::string &GetName() const {
            return name;
        }

        template <typename DoubleType>
        const EdgeScalarList<DoubleType> &GetScalarValues() const;

        ///// Does not provide Derivatives!!!!!!!!!!!!!
        template <typename DoubleType>
        NodeScalarList<DoubleType> GetScalarValuesOnNodes() const;

        ///// Does not provide Derivatives!!!!!!!!!!!!!
        template <typename DoubleType>
        NodeVectorList<DoubleType> GetVectorValuesOnNodes() const;

        const std::vector<size_t> &GetContactIndexes() const;

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
        void SetValues(const EdgeScalarList<DoubleType> &);

        template <typename DoubleType>
        void SetValues(const DoubleType &);

        const Region &GetRegion() const
        {
            return *myregion;
        }

        bool AtContact() const
        {
            return (mycontact != nullptr);
        }

        const Contact &GetContact() const
        {
            return *mycontact;
        }

        const std::string GetContactName() const;

        void SetContact(const ContactPtr);

        EdgeModel::DisplayType GetDisplayType() const
        {
          return displayType;
        }

        const char *GetDisplayTypeString() const
        {
          return DisplayTypeString[static_cast<size_t>(displayType)];
        }

        void SetDisplayType(EdgeModel::DisplayType dt)
        {
          displayType = dt;
        }

        ConstEdgeModelPtr GetConstSelfPtr() const
        {
          return myself.lock();
        }

        EdgeModelPtr GetSelfPtr()
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
        void SetValues(const EdgeScalarList<DoubleType> &) const;

        template <typename DoubleType>
        void SetValues(const DoubleType &) const;

        void MarkOld() const;

    private:
        void CalculateValues() const;
        // Actually performs the computation
        // The nonvirtual method does any required setup.
        //virtual double calcEdgeScalarValue(EdgePtr) const = 0;
        virtual void calcEdgeScalarValues() const = 0;


        EdgeModel();
        EdgeModel(const EdgeModel &);
        EdgeModel &operator=(const EdgeModel &);

        // required for models that store their data
        // (some models may be created on the fly)
        std::string name;
        // need to know my region to get database data and appropriate node and edge lists
        RegionPtr   myregion;
        WeakEdgeModelPtr myself;
        ContactPtr  mycontact;
        mutable bool uptodate;
        mutable bool inprocess;
        mutable std::vector<size_t> atcontact;
        DisplayType displayType;
        mutable ModelDataHolder model_data;
        static const char *DisplayTypeString[];
};

template <typename T1, typename T2, typename ... Args>
EdgeModelPtr create_edge_model(bool use_extended, Args &&...args)
{
  EdgeModel *ret;
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

