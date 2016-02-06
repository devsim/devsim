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

#ifndef AVERAGE_EDGE_MODEL_HH
#define AVERAGE_EDGE_MODEL_HH
#include "EdgeModel.hh"

#include <string>
#include <memory>

class NodeModel;
typedef std::weak_ptr<NodeModel>         WeakNodeModelPtr;
typedef std::shared_ptr<NodeModel>       NodeModelPtr;
typedef std::shared_ptr<const NodeModel> ConstNodeModelPtr;

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

        const std::string   originalEdgeModelName;
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

