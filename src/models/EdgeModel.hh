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

#ifndef EDGEMODEL_HH
#define EDGEMODEL_HH
#include "Vector.hh"

#include "dsmemory.hh"

#include <string>
#include <vector>
#include <iosfwd>

typedef std::vector<double> EdgeScalarList;
typedef std::vector<Vector> EdgeVectorList;
typedef std::vector<double> NodeScalarList;
typedef std::vector<Vector> NodeVectorList;

class EdgeModel;
typedef std::tr1::weak_ptr<EdgeModel>         WeakEdgeModelPtr;
typedef std::tr1::weak_ptr<const EdgeModel>   WeakConstEdgeModelPtr;
typedef std::tr1::shared_ptr<EdgeModel>       EdgeModelPtr;
typedef std::tr1::shared_ptr<const EdgeModel> ConstEdgeModelPtr;

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
        enum DisplayType {NODISPLAY, SCALAR, VECTOR, UNKNOWN};
        EdgeModel(const std::string &, const RegionPtr, EdgeModel::DisplayType, const ContactPtr x = NULL);
        virtual ~EdgeModel();

        const std::string &GetName() const {
            return name;
        }

        // Gets the appropriate value
        // May want to make non-virtual base member and call virtual method within
        // Value is directed from first node in Edge to second node
//      double GetEdgeScalarValue(const Edge *) const;

        const EdgeScalarList &GetScalarValues() const;

        ///// Does not provide Derivatives!!!!!!!!!!!!!
        NodeScalarList GetScalarValuesOnNodes() const;

        ///// Does not provide Derivatives!!!!!!!!!!!!!
        NodeVectorList GetVectorValuesOnNodes() const;

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

        void SetValues(const EdgeScalarList &);
        void SetValues(const double &);

        const Region &GetRegion() const
        {
            return *myregion;
        }

        bool AtContact() const
        {
            return (mycontact != NULL);
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
          return DisplayTypeString[displayType];
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

        const std::string &GetRegionName() const;

        const std::string &GetDeviceName() const;

    protected:
        virtual void Serialize(std::ostream &) const = 0;

        void SerializeBuiltIn(std::ostream &) const;

        void RegisterCallback(const std::string &);

        void SetValues(const EdgeScalarList &) const;
        void SetValues(const double &) const;

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
        mutable bool isuniform;
        mutable double         uniform_value;
        mutable EdgeScalarList values;
        mutable std::vector<size_t> atcontact;
        DisplayType displayType;
        size_t length;
        static const char *DisplayTypeString[];
};

#endif
