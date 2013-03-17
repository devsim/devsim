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

#ifndef DS_INTERPRETER
#define DS_INTERPRETER
#include "ObjectHolder.hh"
#include <string>
#include <vector>

class Interpreter
{
  public:
    Interpreter();
    ~Interpreter();

    bool RunCommand(std::vector<ObjectHolder> &/*command*/);
    bool RunCommand(const std::string &/*command*/, const std::vector<std::string> &/*arguments*/);
    bool RunCommand(const std::string &/*command*/, const std::vector<std::pair<std::string, ObjectHolder> > &/*arguments*/);
    bool RunInternalCommand(const std::string &/*command*/, const std::vector<std::pair<std::string, ObjectHolder> > &/*arguments*/);
    bool RunCommand(const std::string &/*command*/);
    const std::string &GetErrorString() const
    {
      return error_string_;
    }

    ObjectHolder GetResult()
    {
      return result_;
    }

    std::string GetVariable(const std::string &name);


  private:
    Interpreter &operator=(const Interpreter &);
    Interpreter(const Interpreter &);

    std::string   error_string_;
    ObjectHolder  result_;
};
#endif

