/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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
