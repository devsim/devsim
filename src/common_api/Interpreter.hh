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

