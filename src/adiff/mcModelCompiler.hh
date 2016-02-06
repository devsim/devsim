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
