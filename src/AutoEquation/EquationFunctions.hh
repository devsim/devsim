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

#ifndef EQUATION_FUNCTIONS_HH
#define EQUATION_FUNCTIONS_HH
#include "EdgeModel.hh"
#include <string>
#include <utility>

/// No forward declaration of enum
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
ret_pair CreateNodeExprModel(const std::string &/*nm*/, const std::string &/*expr*/, RegionPtr /*rp*/, NodeModel::DisplayType /*dtype*/, ContactPtr cp = NULL);
ret_pair CreateEdgeExprModel(const std::string &/*nm*/, const std::string &/*expr*/, RegionPtr /*rp*/, EdgeModel::DisplayType /*dtype*/, ContactPtr cp = NULL);
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
