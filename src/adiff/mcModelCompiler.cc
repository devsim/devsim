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

#include "mcModelCompiler.hh"

#include <map>
#include <list>
#include <string>
#include <utility>

#include "EquationObject.hh"

std::list<std::pair<std::string, Eqo::EqObjPtr> > ModelList;
std::map<std::string, std::pair<Eqo::EqObjPtr, Eqo::EqObjPtr> > EquationList;

std::list<std::string> ExternalNodeList; // Properties defined outside the model
                                       // Note subckt can know only about ground and nodes at the same level
std::list<std::string> InternalNodeList; // Will require properties (non-negative, etc)

std::map<std::string, std::pair<std::string, double> > ParameterList;

std::map<std::string, std::string> EquationToNodeMap;

std::string ClassName;

/*
   Returns true if the model by the given name is in the ModelList
 */
bool isInModelList(const std::string &str)
{
   std::list<std::pair<std::string, Eqo::EqObjPtr> >::iterator it, end=ModelList.end();
   for (it=ModelList.begin(); it != end; ++it)
   {
      if ((*it).first == str)
         return true;
   }
   return false;
}

/*
   If it is in the model list, this will return a pointer to the model with the given
   name.
 */
Eqo::EqObjPtr findInModelList(const std::string &str)
{
   std::list<std::pair<std::string, Eqo::EqObjPtr> >::iterator it, end=ModelList.end();
   for (it=ModelList.begin(); it != end; ++it)
   {
      if ((*it).first == str)
         return (*it).second;
   }

   // This should not happen
   // a null pointer is bad
   return Eqo::EqObjPtr();
}
