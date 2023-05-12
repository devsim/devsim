/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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

public:
    bool RunCommand(ObjectHolder & /*command*/, std::vector<ObjectHolder> &/*arguments*/);
    bool RunInternalCommand(const std::string &/*command*/, const std::vector<std::pair<std::string, ObjectHolder> > &/*arguments*/);
    bool RunCommand(ObjectHolder & /*command*/, ObjectHolderMap_t &/*arguments*/);

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
    bool RunCommand(const std::string &/*command*/, ObjectHolderMap_t &/*arguments*/);

    Interpreter &operator=(const Interpreter &);
    Interpreter(const Interpreter &);

    std::string   error_string_;
    ObjectHolder  result_;
};
#endif

