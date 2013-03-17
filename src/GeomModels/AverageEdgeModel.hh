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

#ifndef AVERAGE_EDGE_MODEL_HH
#define AVERAGE_EDGE_MODEL_HH
#include "EdgeModel.hh"

#include <string>
#include "dsmemory.hh"

class NodeModel;
typedef std::tr1::weak_ptr<NodeModel>         WeakNodeModelPtr;
typedef std::tr1::shared_ptr<NodeModel>       NodeModelPtr;
typedef std::tr1::shared_ptr<const NodeModel> ConstNodeModelPtr;

#include <string>


/***
 * TODO: This class needs to be broken up into several different classes
 */

class AverageEdgeModel : public EdgeModel
{
    public:
        // This model depends on this Node model to calculate values
        // must match AverageTypeNames below
        enum AverageType_t {ARITHMETIC, GEOMETRIC, GRADIENT, NEGATIVE_GRADIENT, UNKNOWN};

        struct TypeNameMap_t {
          const char *  str;
          AverageType_t type;
        };

        AverageEdgeModel(const std::string &/*edgemodel*/, const std::string &/*nodemodel*/, AverageType_t /*averagetype*/, RegionPtr);
        AverageEdgeModel(const std::string &/*edgemodel*/, const std::string &/*nodemodel*/, const std::string &/*var*/, AverageType_t /*averagetype*/, RegionPtr);
        ~AverageEdgeModel();

        static AverageType_t GetTypeName(const std::string &, std::string &);

        void Serialize(std::ostream &) const;

    private:
        template <typename T>
        void doMath(ConstNodeModelPtr, EdgeScalarList &, const T &) const;
        template <typename T>
        void doMath(ConstNodeModelPtr, ConstNodeModelPtr, EdgeScalarList &, EdgeScalarList &, const T &) const;

        void doGradient(ConstNodeModelPtr, EdgeScalarList &, double) const;
        void doGradient(ConstNodeModelPtr, ConstNodeModelPtr, EdgeScalarList &, EdgeScalarList &, double) const;

        void  calcEdgeScalarValues() const;
        void  setInitialValues();

        const std::string   nodeModelName;
        const std::string   edgeModel1Name;
        const std::string   variableName;
        std::string   derivativeModelName;
        //// derivative with respect to first edge
        mutable WeakEdgeModelPtr node1EdgeModel;
        const AverageType_t averageType;
        static TypeNameMap_t AverageTypeNames[];
};

#endif

