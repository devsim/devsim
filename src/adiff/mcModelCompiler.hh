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

#ifndef MC_MODEL_COMPILER_HH
#define MC_MODEL_COMPILER_HH

#include <map>
#include <string>
#include <list>
#include <utility>

#include "EquationObject.hh"
// Models upon which equations may be built on top of
// order is important, so need to make a true list
extern std::list<std::pair<std::string, Eqo::EqObjPtr> > ModelList;
// The first is dc, then ac term
extern std::map<std::string, std::pair<Eqo::EqObjPtr, Eqo::EqObjPtr> > EquationList;
// The list of variables
//extern std::set<std::string> VariableList;
extern std::list<std::string> ExternalNodeList; // Relatively anonymous, controlled externally
extern std::list<std::string> InternalNodeList; // must know name, update properties, default, etc

// contains name, description, and default value
// also need to add bounds
extern std::map<std::string, std::pair<std::string, double> > ParameterList;

extern std::map<std::string, std::string> EquationToNodeMap;

extern std::string ClassName;

bool isInModelList(const std::string &);

Eqo::EqObjPtr findInModelList(const std::string &);
#endif
