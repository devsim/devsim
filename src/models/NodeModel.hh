/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef NODEMODEL_HH
#define NODEMODEL_HH

#include "ModelDataHolder.hh"

#include <memory>

#include <string>
#include <vector>
#include <iosfwd>

template<typename T>
using NodeScalarList = std::vector<T>;

class NodeModel;

using WeakNodeModelPtr = std::weak_ptr<NodeModel>;
using WeakConstNodeModelPtr = std::weak_ptr<const NodeModel>;
using NodeModelPtr = std::shared_ptr<NodeModel>;
using ConstNodeModelPtr = std::shared_ptr<const NodeModel>;

class Node;
typedef Node *NodePtr;
typedef const Node *ConstNodePtr;

class Contact;
typedef Contact *ContactPtr;
typedef const Contact *ConstContactPtr;

class Region;
typedef Region *RegionPtr;

class NodeModel {
    public:
        enum class DisplayType {NODISPLAY, SCALAR, UNKNOWN};
        NodeModel(const std::string &, const RegionPtr, NodeModel::DisplayType, const ContactPtr x = nullptr);
        virtual ~NodeModel()=0;

        const std::string &GetName() const;

        std::string GetRealName() const;

        std::string GetImagName() const;

        // should collapse this with setNodeScalarValue
        template <typename DoubleType>
        const NodeScalarList<DoubleType> &GetScalarValues() const;

        const std::vector<size_t> &GetContactIndexes() const;

        // recalculate since dependency is invalid
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

        void InitializeValues();

        template <typename DoubleType>
        void SetNodeValue(size_t, DoubleType);

        template <typename DoubleType>
        void SetValues(const NodeScalarList<DoubleType> &);

#if 0
        void SetValues(const NodeModel &);
#endif

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

        NodeModel::DisplayType GetDisplayType() const
        {
          return displayType;
        }

        const char *GetDisplayTypeString() const
        {
          return DisplayTypeString[static_cast<size_t>(displayType)];
        }

        void SetDisplayType(NodeModel::DisplayType dt)
        {
          displayType = dt;
        }

        ConstNodeModelPtr GetConstSelfPtr() const
        {
          return myself.lock();
        }

        NodeModelPtr GetSelfPtr()
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
        //// Devsim Format
        virtual void Serialize(std::ostream &) const = 0;
        void SerializeBuiltIn(std::ostream &) const;

        // Register callbacks in appropriate region
        void RegisterCallback(const std::string &);

        // logically const as is used by the lazy evaluated model values
        template <typename DoubleType>
        void SetValues(const NodeScalarList<DoubleType> &) const;

        template <typename DoubleType>
        void SetValues(const DoubleType &) const;

        void MarkOld() const;

        void DefaultInitializeValues();


    private:
        void CalculateValues() const;

        //virtual DoubleType calcNodeScalarValue(NodePtr) = 0;
        virtual void calcNodeScalarValues() const = 0;
        virtual void setInitialValues() = 0;

        NodeModel();
        NodeModel(const NodeModel &);
        NodeModel &operator=(const NodeModel &);

        std::string name;
        WeakNodeModelPtr myself;
        RegionPtr   myregion;
        ContactPtr  mycontact;
        mutable bool uptodate;
        mutable bool inprocess;
        DisplayType displayType;
        mutable ModelDataHolder model_data;

        mutable std::vector<size_t> atcontact;
        static const char *DisplayTypeString[];
};

template <typename T1, typename T2, typename ... Args>
NodeModelPtr create_node_model(bool use_extended, Args &&...args)
{
  NodeModel *ret;
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

