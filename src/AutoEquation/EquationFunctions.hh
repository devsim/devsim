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

#ifndef EQUATION_FUNCTIONS_HH
#define EQUATION_FUNCTIONS_HH
#include "EdgeModel.hh"
#include <string>
#include <utility>

#include "NodeModel.hh"
#include "EdgeModel.hh"
#include "TriangleEdgeModel.hh"
#include "TetrahedronEdgeModel.hh"

namespace Eqo {
class EquationObject;
typedef std::shared_ptr<EquationObject> EqObjPtr;
}

class Region;
typedef Region *RegionPtr;

class Contact;
typedef Contact *ContactPtr;

class Interface;
typedef Interface *InterfacePtr;

namespace dsHelper {
typedef std::pair<bool, std::string> ret_pair;
ret_pair CreateNodeExprModel(const std::string &/*nm*/, const std::string &/*expr*/, RegionPtr /*rp*/, NodeModel::DisplayType /*dtype*/, ContactPtr cp = nullptr);
ret_pair CreateEdgeExprModel(const std::string &/*nm*/, const std::string &/*expr*/, RegionPtr /*rp*/, EdgeModel::DisplayType /*dtype*/, ContactPtr cp = nullptr);
ret_pair CreateTriangleEdgeExprModel(const std::string &/*nm*/, const std::string &/*expr*/, RegionPtr /*rp*/, TriangleEdgeModel::DisplayType /*dtype*/);
ret_pair CreateTetrahedronEdgeExprModel(const std::string &/*nm*/, const std::string &/*expr*/, RegionPtr /*rp*/, TetrahedronEdgeModel::DisplayType /*dtype*/);
ret_pair CreateInterfaceNodeExprModel(const std::string &/*nm*/, const std::string &/*expr*/, InterfacePtr /*ip*/);
ret_pair SymdiffEval(const std::string &);

NodeModel::DisplayType getNodeModelDisplayType(const std::string &dt);
EdgeModel::DisplayType getEdgeModelDisplayType(const std::string &dt);
TriangleEdgeModel::DisplayType getTriangleEdgeModelDisplayType(const std::string &dt);
TetrahedronEdgeModel::DisplayType getTetrahedronEdgeModelDisplayType(const std::string &dt);
}

#endif
