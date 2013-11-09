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

#ifndef TETRAHEDRON_EDGE_MODEL_HH
#define TETRAHEDRON_EDGE_MODEL_HH

#include <memory>

#include <string>
#include <vector>
#include <iosfwd>

//// Realistically, we should use a struct of the appropriate type
typedef std::vector<double> TetrahedronEdgeScalarList;
typedef std::vector<double> NodeScalarList;
typedef std::vector<double> EdgeScalarList;

class Region;
typedef Region *RegionPtr;

class Edge;
typedef Edge *EdgePtr;
typedef const Edge *ConstEdgePtr;

class EdgeModel;
typedef std::weak_ptr<EdgeModel>         WeakEdgeModelPtr;
typedef std::weak_ptr<const EdgeModel>   WeakConstEdgeModelPtr;
typedef std::shared_ptr<EdgeModel>       EdgeModelPtr;
typedef std::shared_ptr<const EdgeModel> ConstEdgeModelPtr;

class TetrahedronEdgeModel;
typedef std::weak_ptr<TetrahedronEdgeModel>         WeakTetrahedronEdgeModelPtr;
typedef std::weak_ptr<const TetrahedronEdgeModel>   WeakConstTetrahedronEdgeModelPtr;
typedef std::shared_ptr<TetrahedronEdgeModel>       TetrahedronEdgeModelPtr;
typedef std::shared_ptr<const TetrahedronEdgeModel> ConstTetrahedronEdgeModelPtr;


class Tetrahedron;
typedef Tetrahedron *TetrahedronPtr;
typedef const Tetrahedron *ConstTetrahedronPtr;

class TetrahedronEdgeModel {
    public:
        enum DisplayType {NODISPLAY, SCALAR, UNKNOWN};

        TetrahedronEdgeModel(const std::string &, const RegionPtr, TetrahedronEdgeModel::DisplayType);
        virtual ~TetrahedronEdgeModel();

        const std::string &GetName() const {
            return name;
        }

        // Gets the appropriate value
        // May want to make non-virtual base member and call virtual method within
        // Value is directed from first node in Edge to second node
//      double GetTetrahedronScalarValue(const Tetrahedron *) const;
        const TetrahedronEdgeScalarList &GetScalarValues() const;

        enum InterpolationType {AVERAGE, COUPLE, SUM};
        void GetScalarValuesOnNodes(TetrahedronEdgeModel::InterpolationType, std::vector<double> &) const;

        void GetScalarValuesOnElements(std::vector<double> &) const;

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

        void SetValues(const TetrahedronEdgeScalarList &);
        void SetValues(const double &);

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
          return DisplayTypeString[displayType];
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

        void SetValues(const TetrahedronEdgeScalarList &) const;
        void SetValues(const double &) const;

        void MarkOld() const;

    private:
        void CalculateValues() const;

        // Actually performs the computation
        // The nonvirtual method does any required setup.
        //virtual double calcEdgeScalarValue(EdgePtr) const = 0; 
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
        mutable bool isuniform;
        mutable double         uniform_value;
        mutable TetrahedronEdgeScalarList values;
        DisplayType displayType;
        size_t length;
        static const char *DisplayTypeString[];
};

#endif
