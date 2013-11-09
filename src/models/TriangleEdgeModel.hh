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

#ifndef TRIANGLE_EDGE_MODEL_HH
#define TRIANGLE_EDGE_MODEL_HH

#include <memory>
#include <string>
#include <vector>
#include <iosfwd>

typedef std::vector<double> TriangleEdgeScalarList;
typedef std::vector<double> NodeScalarList;
typedef std::vector<double> EdgeScalarList;

class Region;
typedef Region *RegionPtr;

class Edge;
typedef Edge *EdgePtr;
typedef const Edge *ConstEdgePtr;

class EdgeModel;
typedef std::shared_ptr<EdgeModel>       EdgeModelPtr;
typedef std::shared_ptr<const EdgeModel> ConstEdgeModelPtr;

class TriangleEdgeModel;
typedef std::weak_ptr<TriangleEdgeModel>         WeakTriangleEdgeModelPtr;
typedef std::weak_ptr<const TriangleEdgeModel>   WeakConstTriangleEdgeModelPtr;
typedef std::shared_ptr<TriangleEdgeModel>       TriangleEdgeModelPtr;
typedef std::shared_ptr<const TriangleEdgeModel> ConstTriangleEdgeModelPtr;

class Triangle;
typedef Triangle *TrianglePtr;
typedef const Triangle *ConstTrianglePtr;

class TriangleEdgeModel {
    public:
        enum DisplayType {NODISPLAY, SCALAR, UNKNOWN};
        TriangleEdgeModel(const std::string &, const RegionPtr, TriangleEdgeModel::DisplayType);
        virtual ~TriangleEdgeModel();

        const std::string &GetName() const {
            return name;
        }

        // Gets the appropriate value
        // May want to make non-virtual base member and call virtual method within
        // Value is directed from first node in Edge to second node
//      double GetTriangleScalarValue(const Triangle *) const;
        const TriangleEdgeScalarList &GetScalarValues() const;

        enum InterpolationType {AVERAGE, COUPLE, SUM};
        void GetScalarValuesOnNodes(TriangleEdgeModel::InterpolationType, std::vector<double> &) const;

        void GetScalarValuesOnElements(std::vector<double> &) const;

        //// Sum up values on sides of the edge
        EdgeScalarList GetValuesOnEdges() const;

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

        void SetValues(const TriangleEdgeScalarList &);
        void SetValues(const double &);

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
          return DisplayTypeString[displayType];
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

        void SetValues(const TriangleEdgeScalarList &) const;
        void SetValues(const double &) const;

        void MarkOld() const;

    private:
        void CalculateValues() const;

        // Actually performs the computation
        // The nonvirtual method does any required setup.
        //virtual double calcEdgeScalarValue(EdgePtr) const = 0; 
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
        mutable bool isuniform;
        mutable double         uniform_value;
        mutable TriangleEdgeScalarList values;
        DisplayType displayType;
        size_t length;
        static const char *DisplayTypeString[];
};

#endif
