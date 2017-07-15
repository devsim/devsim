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

#ifndef TRIANGLE_EDGE_EXPR_MODEL_HH
#define TRIANGLE_EDGE_EXPR_MODEL_HH
#include "TriangleEdgeModel.hh"
#include <string>
#include <memory>
namespace Eqo {
    class EquationObject;
    typedef std::shared_ptr<EquationObject> EqObjPtr;

}

template <typename DoubleType>
class TriangleEdgeExprModel : public TriangleEdgeModel
{
    public:
        TriangleEdgeExprModel(const std::string &, Eqo::EqObjPtr, RegionPtr, TriangleEdgeModel::DisplayType);

        void Serialize(std::ostream &) const;

    private:
        void RegisterModels();
        TriangleEdgeExprModel();
        TriangleEdgeExprModel(const TriangleEdgeExprModel &);

        void calcTriangleEdgeScalarValues() const;

        const Eqo::EqObjPtr      equation;
};

#endif
