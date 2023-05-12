/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef INTERFACENODEMODEL_HH
#define INTERFACENODEMODEL_HH

#include "ModelDataHolder.hh"

#include <string>
#include <vector>

#include <memory>

template<typename T>
using NodeScalarList = std::vector<T>;

class InterfaceNodeModel;

using WeakInterfaceNodeModelPtr = std::weak_ptr<InterfaceNodeModel>;
using WeakConstInterfaceNodeModelPtr = std::weak_ptr<const InterfaceNodeModel>;
using InterfaceNodeModelPtr = std::shared_ptr<InterfaceNodeModel>;
using ConstInterfaceNodeModelPtr = std::shared_ptr<const InterfaceNodeModel>;

class Interface;
typedef Interface *InterfacePtr;

/// This class has models which are always out of date and are always recalculated
class InterfaceNodeModel {
    public:
        InterfaceNodeModel(const std::string &, const InterfacePtr);
        virtual ~InterfaceNodeModel()=0;

        const std::string &GetName() const {
            return name;
        }

        // should collapse this with setNodeScalarValue
        template <typename DoubleType>
        const NodeScalarList<DoubleType> &GetScalarValues() const;

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
        void SetValues(const NodeScalarList<DoubleType> &);

#if 0
        template <typename DoubleType>
        void SetValues(const InterfaceNodeModel &);
#endif

        const Interface &GetInterface() const
        {
            return *myinterface;
        }

        ConstInterfaceNodeModelPtr GetConstSelfPtr() const
        {
          return myself.lock();
        }

        InterfaceNodeModelPtr GetSelfPtr()
        {
          return myself.lock();
        }

        bool IsUniform() const;

        template <typename DoubleType>
        const DoubleType &GetUniformValue() const;

        size_t GetLength() const
        {
          return length;
        }

        bool IsZero() const;

        bool IsOne() const;

        void DevsimSerialize(std::ostream &) const;

        const std::string &GetDeviceName() const;

        const std::string &GetInterfaceName() const;

    protected:
        virtual void Serialize(std::ostream &) const = 0;

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
        virtual void   calcNodeScalarValues() const = 0;

        InterfaceNodeModel();
        InterfaceNodeModel(const InterfaceNodeModel &);
        InterfaceNodeModel &operator=(const InterfaceNodeModel &);

        std::string name;
        WeakInterfaceNodeModelPtr myself;
        InterfacePtr   myinterface;
        mutable bool uptodate = false;
        mutable bool inprocess = false;
        mutable ModelDataHolder model_data;
        size_t length = 0;
};

template <typename T1, typename T2, typename ... Args>
InterfaceNodeModelPtr create_interface_node_model(bool use_extended, Args &&...args)
{
  InterfaceNodeModel *ret;
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

