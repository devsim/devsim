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

#ifndef COMMAND_HANDLER_HH
#define COMMAND_HANDLER_HH

#include "ObjectHolder.hh"
#include <string>
#include <vector>
#include <map>

class ObjectHolder;
class CommandHandler;
namespace dsGetArgs
{
/// Rules,
/**
 * A switch can be for any type.  Specify mutual exclusion of boolean or string options
 */

/// The convention of a default existing means that non-specification is not an error
/**
 * Rules:
 * Switches without default require the user to make a selection
 *
 * StringOptions
 */
/// **switchList : the value for this switch
/// *switchList  : a switch list
/// switchList   : a list of list of switches
//typedef const char * switchDefault;

enum class optionType {BOOLEAN = 0, STRING, INTEGER, FLOAT, LIST};
enum class requiredType {OPTIONAL, REQUIRED};
namespace optionTypeString {
extern const char * optionTypeStrings[];
}


typedef bool (*checkFunction)(const std::string &/*optionname*/, std::string &/*errorString*/, const CommandHandler &);

struct Option {
    const char *name;
    //// always use "" instead of nullptr
    const char   *defaultValue;
    optionType    type;
    requiredType  rtype;
    checkFunction func;
};

typedef const char *** switchList;
typedef Option       * optionList;

}

class CommandHandler;
typedef void (*newcmd)(CommandHandler &);


class CommandHandler
{
  public:
    CommandHandler(void *command_info);
    ~CommandHandler();
    void SetErrorResult(const std::string &msg);
    void SetEmptyResult();
    const std::string &GetCommandName() const;

    void SetObjectResult(ObjectHolder);
    void SetStringListResult(const std::vector<std::string> &list);
    void SetIntListResult(const std::vector<int> &list);
    void SetDoubleListResult(const std::vector<double> &list);
    void SetStringResult(const std::string &);
    void SetDoubleResult(double v);
    void SetIntResult(int v);
    void SetListResult(ObjectHolderList_t &object_list);
    void SetMapResult(ObjectHolderMap_t &object_list);

    void *GetCommandInfo()
    {
      return command_info_;
    }

    int GetReturnCode() const
    {
      return return_code_;
    }

    ObjectHolder GetReturnObject() const
    {
      return return_object_;
    }

    bool        processOptions(dsGetArgs::optionList opts, dsGetArgs::switchList sl, std::string &) const;
    std::string GetStringOption(const std::string &) const;
    double      GetDoubleOption(const std::string &) const;
    int         GetIntegerOption(const std::string &) const;
    bool        GetBooleanOption(const std::string &) const;
    ObjectHolder GetObjectHolder(const std::string &) const;


  private:


    void        *command_info_;
    std::string  command_name;
    int          return_code_;
    ObjectHolder return_object_;
};

template <typename T, typename U> std::vector<T> GetKeys(const std::map<T, U> &tmap)
{
  std::vector<T> result;
  for (typename std::map<T, U>::const_iterator it = tmap.begin(); it != tmap.end(); ++it)
  {
    result.push_back(it->first);
  }
  return result;
}
#endif

