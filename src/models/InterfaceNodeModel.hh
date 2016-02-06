/***
DEVSIM
Copyright 2013 Devsim LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
***/

#ifndef INTERFACENODEMODEL_HH
#define INTERFACENODEMODEL_HH

#include <string>
#include <vector>

#include <memory>

typedef std::vector<double> NodeScalarList;

class InterfaceNodeModel;
typedef std::weak_ptr<InterfaceNodeModel>       WeakInterfaceNodeModelPtr;
typedef std::weak_ptr<const InterfaceNodeModel> WeakConstInterfaceNodeModelPtr;
typedef std::shared_ptr<InterfaceNodeModel>       InterfaceNodeModelPtr;
typedef std::shared_ptr<const InterfaceNodeModel> ConstInterfaceNodeModelPtr;

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
        const NodeScalarList &GetScalarValues() const;

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

        void SetValues(const NodeScalarList &);
        void SetValues(const InterfaceNodeModel &);

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

        double GetUniformValue() const;

        size_t GetLength() const
        {
          return length;
        }

        bool IsZero() const
        {
          return (IsUniform() && (uniform_value == 0.0));
        }

        bool IsOne() const
        {
          return (IsUniform() && (uniform_value == 1.0));
        }

        void DevsimSerialize(std::ostream &) const;

        const std::string &GetDeviceName() const;

        const std::string &GetInterfaceName() const;

    protected:
        virtual void Serialize(std::ostream &) const = 0;

        // Register callbacks in appropriate region
        void RegisterCallback(const std::string &);

        // logically const as is used by the lazy evaluated model values
        void SetValues(const NodeScalarList &) const;
        void SetValues(const double &) const;

        void MarkOld() const;

        void DefaultInitializeValues();

    private:
        void CalculateValues() const;
        //virtual double calcNodeScalarValue(NodePtr) = 0; 
        virtual void   calcNodeScalarValues() const = 0; 

        InterfaceNodeModel();
        InterfaceNodeModel(const InterfaceNodeModel &);
        InterfaceNodeModel &operator=(const InterfaceNodeModel &);

        std::string name;
        WeakInterfaceNodeModelPtr myself;
        InterfacePtr   myinterface;
        mutable bool uptodate;
        mutable bool inprocess;
        mutable bool isuniform;
        mutable double         uniform_value;
        mutable      NodeScalarList values;
        size_t length;
};
#endif
